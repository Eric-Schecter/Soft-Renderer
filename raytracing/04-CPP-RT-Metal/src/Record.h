#pragma once

#include <glm/vec3.hpp>
#include <limits>
#include <memory>

#include "Material.h"

class Material;

struct Record {
	bool isHit = false;
	float t = std::numeric_limits<float>::infinity();
	glm::vec3 pos;
	glm::vec3 normal;
	std::shared_ptr<Material> material;
};