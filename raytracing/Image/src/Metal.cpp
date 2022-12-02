#include "Metal.h"

#include <glm/geometric.hpp>

#include "utils.h"

bool Metal::scatter(const Ray& ray, const Record& record, Ray& scatteredRay, glm::vec3& attenuation) {
	glm::vec3 reflected = glm::reflect(ray.direction,record.normal);
	scatteredRay = Ray(record.pos, glm::normalize(reflected + fuzz * utils::getRandomUnitVecByNormal(record.normal)));
	attenuation = texture->sample(record.uv,record.pos);
	return glm::dot(scatteredRay.direction,record.normal)>0.f;
}