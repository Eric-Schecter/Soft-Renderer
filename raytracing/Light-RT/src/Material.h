#pragma once

#include "Ray.h"
#include "Record.h"

#include <glm/vec3.hpp>

struct Record;

class Material {
public:
	virtual glm::vec3 emitted(const glm::vec2& uv, const glm::vec3& p) const {
		return glm::vec3(0.f);
	}
	virtual bool scatter(const Ray& ray, const Record& record, Ray& scatteredRay, glm::vec3& attenuation) const = 0;
};