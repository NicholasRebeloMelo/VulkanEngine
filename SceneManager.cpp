#include "SceneManager.hpp"
//std
#include <stdexcept>
#include <array>
namespace ve {

	SceneManager::SceneManager()
	{
		createPipelineLayout();
		createPipeline();
		createCommandBuffers();
	}

	SceneManager::~SceneManager()
	{
		vkDestroyPipelineLayout(veDevice.device(), pipelineLayout, nullptr);
	}

	void SceneManager::run()
	{
		while (!veWindow.shouldClose())
		{
			glfwPollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(veDevice.device());
	}
	void SceneManager::createPipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(veDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}
	void SceneManager::createPipeline()
	{
		auto pipelineConfig = VePipeline::defaultPipelineConfigInfo(veSwapChain.width(), veSwapChain.height());
		pipelineConfig.renderPass = veSwapChain.getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		vePipeline = std::make_unique<VePipeline>(veDevice, "shaders/simpleShader.vert.spv", "shaders/simpleShader.frag.spv", pipelineConfig);
	}

	void SceneManager::createCommandBuffers()
	{
		commandBuffers.resize(veSwapChain.imageCount());
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = veDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(veDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}

		for(int i = 0; i < commandBuffers.size(); i++)
		{
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			
			if(vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("failed to begin recording command buffer !");
			}

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = veSwapChain.getRenderPass();
			renderPassInfo.framebuffer = veSwapChain.getFrameBuffer(i);
			
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = veSwapChain.getSwapChainExtent();

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f };
			clearValues[1].depthStencil = {1.0f, 0};
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			vePipeline->bind(commandBuffers[i]);
			vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

			vkCmdEndRenderPass(commandBuffers[i]);
			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to record command buffer!");
			}
		}
	}

	void SceneManager::drawFrame()
	{
		uint32_t imageIndex;
		auto result = veSwapChain.acquireNextImage(&imageIndex);
		
		if( result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR ){
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		result = veSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		
		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to prevent swap chain image !");
		}
	}
}