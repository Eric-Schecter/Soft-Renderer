#pragma once

#include "Primitive.h"

class Box : public Primitive
{
public:
	Box(const glm::vec3& bmin, const glm::vec3& bmax, const std::shared_ptr<Material>& _material);

	bool hit(const Ray& ray, Record& record);

private:
	glm::vec3 getNormal(const glm::vec3& p);
	glm::vec2 getUV(const glm::vec3& p);

	glm::vec3 bounds[2];
	std::shared_ptr<Material> material;
};

