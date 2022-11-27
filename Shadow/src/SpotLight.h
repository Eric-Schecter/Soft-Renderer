#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <memory>

#include "DepthTexture.h"

struct SpotLight
{
	glm::vec3 color;
	glm::vec3 pos;
	glm::vec3 direction;
	float intensity;
	float cutOff;
	float outerCutOff;
	glm::mat4 shadowMatrix;
	std::shared_ptr<DepthTexture> shadowmap;
};