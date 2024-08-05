#pragma once

#include "ve_device.hpp"
#include "Window.hpp"

// libs

#include <imgui.h>
#include <../imgui-master/imgui/backends/imgui_impl_glfw.h>
#include <../imgui-master/imgui/backends/imgui_impl_vulkan.h>

// std
#include <stdexcept>

// This whole class is only necessary right now because it needs to manage the descriptor pool
// because we haven't set one up anywhere else in the application, and we manage the
// example state, otherwise all the functions could just be static helper functions if you prefered
namespace ve {

	static void check_vk_result(VkResult err) {
		if (err == 0) return;
		fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
		if (err < 0) abort();
	}

	class VeImgui {
	public:
		VeImgui(VeWindow &window, VeDevice &device, VkRenderPass renderPass, uint32_t imageCount);
		~VeImgui();

		void newFrame();

		void render(VkCommandBuffer commandBuffer);

		// Example state
		bool show_demo_window = true;
		bool show_another_window = false;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		void runExample();

	private:
		VeDevice &veDevice;

		VkDescriptorPool descriptorPool;
	};
}  // namespace lve