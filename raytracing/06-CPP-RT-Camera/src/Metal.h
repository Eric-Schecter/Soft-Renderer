#pragma once

#include "Material.h"

class Metal:public Material
{
public:
	Metal(const glm::vec3& _color,float _fuzz = 0.f):color(_color),fuzz(std::min(_fuzz,1.f)) {};

	bool scatter(const Ray& ray, const Record& record, Ray& scatteredRay, glm::vec3& attenuation) override;

	glm::vec3 color;
	float fuzz;
};

