#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec3 tangent;
	glm::vec3 biTangent;
};

