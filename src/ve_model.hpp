#pragma once
#include "ve_device.hpp"
//glm libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>
//std
#include <vector>
namespace ve {
	class VeModel {
	public:

		struct Vertex {
			glm::vec3 position;
			glm::vec3 color;


			static std::vector<VkVertexInputBindingDescription> getBindingDesciptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDesciptions();
		};

		struct Builder {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};
		};

		VeModel(VeDevice &device, const VeModel::Builder &builder);
		~VeModel();

		VeModel(const VeModel &) = delete;
		VeModel &operator=(const VeModel&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);
	private:
		void createVertexBuffers(const std::vector<Vertex> &vertices);
		void createIndexBuffer(const std::vector<uint32_t> &indices);

		VeDevice &veDevice;

		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;

		bool hasIndexBuffer = false;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		uint32_t indexCount;

	};
}