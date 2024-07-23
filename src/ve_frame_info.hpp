#pragma once

#include "ve_camera.hpp"
#include "ve_gameobject.hpp"

// lib
#include <vulkan/vulkan.h>

namespace ve {
	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		VeCamera &camera;
		VkDescriptorSet globalDescriptorSet;
		VeGameObject::Map &gameObjects;
	};
}