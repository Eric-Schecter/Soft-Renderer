#pragma once

#include "Ray.h"
#include "Record.h"

#include <glm/vec3.hpp>

struct Record;

class Material {
public:
	virtual bool scatter(const Ray& ray, const Record& record, Ray& scatteredRay, glm::vec3& attenuation) = 0;

protected:
	float random(float min, float max);
	glm::vec3 getRandomUnitVec();
	glm::vec3 getRandomUnitVecByNormal(const glm::vec3& normal);
	bool checkFrontFace(const glm::vec3& rayDir, const glm::vec3& normal);
};