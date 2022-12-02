#pragma once

#include "Ray.h"
#include "Record.h"

#include <glm/vec3.hpp>

struct Record;

class Material {
public:
	virtual bool scatter(const Ray& ray, const Record& record, Ray& scatteredRay, glm::vec3& attenuation) = 0;
};