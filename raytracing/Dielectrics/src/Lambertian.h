#pragma once

#include "Material.h"
#include "Record.h"
#include "Ray.h"

#include <glm/vec3.hpp>

class Lambertian:public Material
{
public:
	Lambertian(const glm::vec3& _color) :color(_color) {};

	bool scatter(const Ray& ray, const Record& record, Ray& scatteredRay, glm::vec3& attenuation) override;

	glm::vec3 color;
};

