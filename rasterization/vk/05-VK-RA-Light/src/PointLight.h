#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct PointLight {
	glm::vec4 color;
	glm::vec3 pos;
	float intensity;
	float constant;
	float linear;
	float quadratic;
};