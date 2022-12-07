#pragma once

#include "Primitive.h"
#include "Ray.h"
#include "Material.h"
#include "Record.h"

#include <glm/vec3.hpp>
#include <memory>

class Sphere:public Primitive
{
public:
	Sphere(const glm::vec3& _center, float _radius, const std::shared_ptr<Material>& _material) :center(_center), radius(_radius), material(_material) {};

	bool hit(const Ray& ray, Record& record) override;
	glm::vec2 getUV(const glm::vec3& p);

	glm::vec3 center;
	float radius;
	std::shared_ptr<Material> material;
};

