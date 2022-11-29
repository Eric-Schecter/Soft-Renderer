#include "Metal.h"

#include <glm/geometric.hpp>

bool Metal::scatter(const Ray& ray, const Record& record, Ray& reflectedRay, glm::vec3& attenuation) {
	glm::vec3 reflected = glm::reflect(ray.direction,record.normal);
	reflectedRay = Ray(record.pos, glm::normalize(reflected + fuzz * getRandomUnitVecByNormal(record.normal)));
	attenuation = color;
	return glm::dot(reflectedRay.direction,record.normal)>0.f;
}