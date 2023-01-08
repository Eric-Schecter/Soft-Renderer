#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct DirectionalLight {
	glm::vec4 color;
	glm::vec3 direction;
	float intensity;
};