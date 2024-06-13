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

    // temporary helper function, creates a 1x1x1 cube centered at offset
    std::unique_ptr<VeModel> createCubeModel(VeDevice& device, glm::vec3 offset) {
        VeModel::Builder modelBuilder{};
        modelBuilder.vertices = {
            // left face (white)
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

            // right face (yellow)
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

            // top face (orange, remember y axis points down)
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

            // bottom face (red)
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

            // nose face (blue)
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

            // tail face (green)
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        };
        for (auto& v : modelBuilder.vertices) {
            v.position += offset;
        }

        modelBuilder.indices = { 0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
                                12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };

        return std::make_unique<VeModel>(device, modelBuilder);
    }


	void SceneManager::loadGameObjects()
	{
        std::shared_ptr<VeModel> veModel = createCubeModel(veDevice, {0.0f, 0.0f, 0.0f});

        auto cube = VeGameObject::createGameObject();
        cube.model = veModel;
        cube.transform.translation = {0.0f, 0.0f, 2.5f};
        cube.transform.scale = {0.5f, 0.5f, 0.5f};
        gameObjects.push_back(std::move(cube));
	}
}