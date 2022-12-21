#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vulkan/vulkan.hpp>
#include <array>

struct Vertex {
	glm::vec2 position;
	glm::vec3 color;

	static vk::VertexInputBindingDescription getBindingDescription() {
		vk::VertexInputBindingDescription bindingDescription = {
			0,
			sizeof(Vertex),
			vk::VertexInputRate::eVertex
		};
		return bindingDescription;
	}

	static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions() {
		std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions{
			vk::VertexInputAttributeDescription{
				0,
				0,
				vk::Format::eR32G32Sfloat,
				offsetof(Vertex,position)
			},
			vk::VertexInputAttributeDescription{
				1,
				0,
				vk::Format::eR32G32B32Sfloat,
				offsetof(Vertex,color)
			},
		};
		return attributeDescriptions;
	}
};
