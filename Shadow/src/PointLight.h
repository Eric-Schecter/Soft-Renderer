#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "DepthTexture.h"

struct PointLight
{
	glm::vec3 color;
	glm::vec3 pos;
	float intensity;
	float constant;
	float linear;
	float quadratic;
	glm::mat4 shadowMatrix[6];
	DepthTexture shadowmap[6];
};
