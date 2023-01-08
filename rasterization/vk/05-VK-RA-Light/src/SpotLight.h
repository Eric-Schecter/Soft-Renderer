#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct SpotLight {
	glm::vec4 color;
	glm::vec3 pos;
	float cutOff;
	glm::vec3 direction;
	float outerCutOff;
	float intensity;
};