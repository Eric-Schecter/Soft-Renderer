#pragma once

#include <glm/vec3.hpp>
#include <limits>

struct Record {
	bool isHit = false;
	float t = std::numeric_limits<float>::infinity();
	glm::vec3 pos;
	glm::vec3 normal;
};