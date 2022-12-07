#include "Lambertian.h"

#include <glm/vec3.hpp>
#include <glm/geometric.hpp>

bool Lambertian::scatter(const Ray& ray,const Record& record, Ray& reflectedRay, glm::vec3& attenuation) {
	glm::vec3 target(record.pos + record.normal + getRandomUnitVec());
	glm::vec3 direction = target - record.pos;
	const float limit = 1e-4;
	if (glm::length(direction)<limit) {
		direction = record.normal; // avoid zero reflected direction
	}
	direction = glm::normalize(direction);

	reflectedRay = Ray(record.pos, direction);
	attenuation = color;
	return true;
}