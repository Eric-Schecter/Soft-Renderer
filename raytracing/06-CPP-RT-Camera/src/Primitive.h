#pragma once

#include "Ray.h"
#include "Record.h"

#include <glm/geometric.hpp>

class Primitive 
{
public:
	virtual ~Primitive() {};

	virtual bool hit(const Ray& ray, Record& record) = 0;

	inline bool checkFrontFace(const glm::vec3& rayDir, const glm::vec3& normal) {
		return glm::dot(rayDir, normal) < 0.f;
	}
};