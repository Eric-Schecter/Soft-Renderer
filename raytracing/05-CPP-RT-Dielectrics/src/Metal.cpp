#include "Metal.h"

#include <glm/geometric.hpp>

bool Metal::scatter(const Ray& ray, const Record& record, Ray& scatteredRay, glm::vec3& attenuation) {
	glm::vec3 reflected = glm::reflect(ray.direction,record.normal);
	scatteredRay = Ray(record.pos, glm::normalize(reflected + fuzz * getRandomUnitVecByNormal(record.normal)));
	attenuation = color;
	return glm::dot(scatteredRay.direction,record.normal)>0.f;
}