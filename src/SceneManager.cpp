#include "SceneManager.hpp"

#include "keyboard_movement_controller.hpp"
#include "mouse_movement_controller.hpp"
#include"ve_camera.hpp"
#include "systems/simple_render_system.hpp"
#include "systems/point_light_system.hpp"
#include "ve_buffer.hpp"

//glm libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>
#include <glm/gtc/constants.hpp>
//std
#include <stdexcept>
#include <chrono>
#include <array>
#include <cassert>
namespace ve {

	
	

	SceneManager::SceneManager()
	{
		globalPool = 
			VeDescriptorPool::Builder(veDevice)
			.setMaxSets(VeSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VeSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadGameObjects();
		
	}

	SceneManager::~SceneManager() {}

	void SceneManager::run()
	{
		std::vector<std::unique_ptr<VeBuffer>> uboBuffers(VeSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<VeBuffer>(
				veDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			uboBuffers[i]->map();
		}

		auto globalSetLayout = 
			 VeDescriptorSetLayout::Builder(veDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(VeSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			VeDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem{
			veDevice, 
			veRenderer.getSwapChainRenderPass(), 
			globalSetLayout->getDescriptorSetLayout()};

		PointLightSystem pointLightSystem{
			veDevice,
			veRenderer.getSwapChainRenderPass(),
			globalSetLayout->getDescriptorSetLayout() };
        VeCamera camera{};
        camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

        auto viewerObject = VeGameObject::createGameObject();
		viewerObject.transform.translation.z = -2.5f;

        KeyboardMovementController cameraController{};
		MouseMovementController mouseController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

		while (!veWindow.shouldClose())
		{
			glfwPollEvents();

            glfwSetInputMode(veWindow.getGLFWwindow(), GLFW_CURSOR_DISABLED, GLFW_TRUE); 
			

			GLFWwindow *window = veWindow.getGLFWwindow();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

           // frameTime = glm::min(frameTime, MAX_FRAME_TIME);
			

           cameraController.moveInPlaneXZ(veWindow.getGLFWwindow(), frameTime, viewerObject);
		   mouseController.look(veWindow.getGLFWwindow(), frameTime, viewerObject);

		   

            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = veRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

			if (auto commandBuffer = veRenderer.beginFrame()) {
				int frameIndex = veRenderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex],
					gameObjects				
				};


				//update
				GlobalUbo ubo{};
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				ubo.inverseView = camera.getInverseView();
				pointLightSystem.update(frameInfo, ubo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();
			
				
				//render
				veRenderer.beginSwapChainRenderPass(commandBuffer);
				//order matters here
				simpleRenderSystem.renderGameObjects(frameInfo);
				pointLightSystem.render(frameInfo);

				veRenderer.endSwapChainRenderPass(commandBuffer);
				veRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(veDevice.device());
	}

	void SceneManager::loadGameObjects()
	{
        std::shared_ptr<VeModel> veModel = VeModel::createModelFromFile(veDevice, "models/smooth_vase.obj");
        auto smoothVase = VeGameObject::createGameObject();
		smoothVase.model = veModel;
		smoothVase.transform.translation = {-0.5f, 0.5f, 0.f};
		smoothVase.transform.scale = {3.f, 1.5f, 3.f};
		gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

		veModel = VeModel::createModelFromFile(veDevice, "models/flat_vase.obj");
		auto flatVase = VeGameObject::createGameObject();
		flatVase.model = veModel;
		flatVase.transform.translation = { 0.5f, 0.5f, 0.f };
		flatVase.transform.scale = {3.f, 1.5f, 3.f};
		gameObjects.emplace(flatVase.getId(), std::move(flatVase));

		veModel = VeModel::createModelFromFile(veDevice, "models/quad.obj");
		auto floor = VeGameObject::createGameObject();
		floor.model = veModel;
		floor.transform.translation = { 0.f, 0.5f, 0.f };
		floor.transform.scale = { 3.f, 1.f, 3.f };
		gameObjects.emplace(floor.getId(), std::move(floor));

		

		std::vector<glm::vec3> lightColors{
		{1.f, .1f, .1f},
		{.1f, .1f, 1.f},
		{.1f, 1.f, .1f},
		{1.f, 1.f, .1f},
		{.1f, 1.f, 1.f},
		{1.f, 1.f, 1.f}  //
		};
		for (int i = 0; i < lightColors.size(); i++) {
			auto pointLight = VeGameObject::makePointLight(0.2f);
			pointLight.color = lightColors[i];
			auto rotateLight = glm::rotate(
				glm::mat4(1.f),
				(i * glm::two_pi<float>()) / lightColors.size(),
				{ 0.f, -1.f, 0.f });
			pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
			gameObjects.emplace(pointLight.getId(), std::move(pointLight));
		}
	}
}