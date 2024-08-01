#pragma once

#include "Window.hpp"
#include "ve_descriptors.hpp"
#include "ve_device.hpp"
#include "ve_gameobject.hpp"
#include "ve_renderer.hpp"


#include "GLFW/glfw3.h"



//std lib
#include <memory>
#include <vector>
namespace ve 
{
	

	class SceneManager 
	{
	public: 
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT	 = 600;

		SceneManager();
		~SceneManager();

		SceneManager(const SceneManager&) = delete;
		SceneManager& operator=(const SceneManager&) = delete;

		void run();

	private:
		void loadGameObjects();

		
		
		VeWindow veWindow{ WIDTH, HEIGHT, "Nick's Vulkan Engine" };
		VeDevice veDevice{veWindow};
		VeRenderer veRenderer{veWindow, veDevice};

		// order of declaration matter
		std::unique_ptr<VeDescriptorPool> globalPool{};
		VeGameObject::Map gameObjects;
	
	};
}