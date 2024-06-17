#include "SceneManager.hpp"

#include "keyboard_movement_controller.hpp"
#include "mouse_view_controller.hpp"
#include"ve_camera.hpp"
#include "simple_render_system.hpp"

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
		loadGameObjects();
		
	}

	SceneManager::~SceneManager() {}

	void SceneManager::run()
	{
		SimpleRenderSystem simpleRenderSystem{veDevice, veRenderer.getSwapChainRenderPass()};
        VeCamera camera{};
        camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

        auto viewerObject = VeGameObject::createGameObject();

        KeyboardMovementController cameraController{};
        MouseViewController mouseController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

		while (!veWindow.shouldClose())
		{
			glfwPollEvents();

            glfwSetInputMode(veWindow.getGLFWwindow(), GLFW_CURSOR_DISABLED, GLFW_TRUE);  // Capture cursor now

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

           // frameTime = glm::min(frameTime, MAX_FRAME_TIME);

            cameraController.moveInPlaneXZ(veWindow.getGLFWwindow(), frameTime, viewerObject);
            mouseController.updateFromMouse(veWindow.getGLFWwindow(), frameTime, viewerObject);
            
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = veRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);

			if (auto commandBuffer = veRenderer.beginFrame()) {
				veRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
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
		smoothVase.transform.translation = {-0.5f, 0.5f, 2.5f};
		smoothVase.transform.scale = {3.f, 1.5f, 3.f};
		gameObjects.push_back(std::move(smoothVase));

		veModel = VeModel::createModelFromFile(veDevice, "models/flat_vase.obj");
		auto flatVase = VeGameObject::createGameObject();
		flatVase.model = veModel;
		flatVase.transform.translation = { 0.5f, 0.5f, 2.5f };
		flatVase.transform.scale = {3.f, 1.5f, 3.f};
		gameObjects.push_back(std::move(flatVase));
	}
}