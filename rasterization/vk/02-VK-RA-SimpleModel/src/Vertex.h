#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vulkan/vulkan.hpp>
#include <array>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec3 tangent;
	glm::vec3 biTangent;

	static vk::VertexInputBindingDescription getBindingDescription() {
		vk::VertexInputBindingDescription bindingDescription = {
			0,
			sizeof(Vertex),
			vk::VertexInputRate::eVertex
		};
		return bindingDescription;
	}

	static std::array<vk::VertexInputAttributeDescription, 5> getAttributeDescriptions() {
		std::array<vk::VertexInputAttributeDescription, 5> attributeDescriptions{
			vk::VertexInputAttributeDescription{
				0,
				0,
				vk::Format::eR32G32B32Sfloat,
				offsetof(Vertex,position)
			},
			vk::VertexInputAttributeDescription{
				1,
				0,
				vk::Format::eR32G32B32Sfloat,
				offsetof(Vertex,normal)
			},
			vk::VertexInputAttributeDescription{
				2,
				0,
				vk::Format::eR32G32Sfloat,
				offsetof(Vertex,uv)
			},
			vk::VertexInputAttributeDescription{
				3,
				0,
				vk::Format::eR32G32B32Sfloat,
				offsetof(Vertex,tangent)
			},
			vk::VertexInputAttributeDescription{
				4,
				0,
				vk::Format::eR32G32B32Sfloat,
				offsetof(Vertex,biTangent)
			},
		};
		return attributeDescriptions;
	}
};
