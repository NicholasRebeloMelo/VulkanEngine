#pragma once
#include "Window.hpp"
#include "ve_pipeline.hpp"
#include "ve_swap_chain.hpp"
#include "ve_device.hpp"
#include "ve_model.hpp"

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
		void loadModels();
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);

		VeWindow veWindow{ WIDTH, HEIGHT, "Nick's Vulkan Engine" };
		VeDevice veDevice{veWindow};
		std::unique_ptr<VeSwapChain> veSwapChain;
		std::unique_ptr<VePipeline> vePipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::unique_ptr<VeModel> veModel;
	};
}