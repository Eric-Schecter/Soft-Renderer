#pragma once

#include "Primitive.h"
#include "Ray.h"

#include <glm/vec3.hpp>

class Sphere:public Primitive
{
public:
	Sphere(const glm::vec3& _center, float _radius, const glm::vec3& _color) :center(_center), radius(_radius), color(_color) {};

	bool hit(const Ray& ray, Record& record) override;

	glm::vec3 center;
	float radius;
	glm::vec3 color;
};

