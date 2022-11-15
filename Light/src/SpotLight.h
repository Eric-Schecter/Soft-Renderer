#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct SpotLight
{
	glm::vec4 color;
	glm::vec3 pos;
	glm::vec3 direction;
	float intensity;
	float cutOff;
	float outerCutOff;
};