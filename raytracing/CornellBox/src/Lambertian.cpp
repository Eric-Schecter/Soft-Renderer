#include "Lambertian.h"

#include <glm/vec3.hpp>
#include <glm/geometric.hpp>

#include "utils.h"

bool Lambertian::scatter(const Ray& ray,const Record& record, Ray& scatteredRay, glm::vec3& attenuation) const {
	glm::vec3 target(record.pos + record.normal + utils::getRandomUnitVec());
	glm::vec3 direction = target - record.pos;
	const float limit = 1e-4f;
	if (glm::length(direction)<limit) {
		direction = record.normal; // avoid zero reflected direction
	}
	direction = glm::normalize(direction);

	scatteredRay = Ray(record.pos, direction);
	attenuation = texture->sample(record.uv,record.pos);
	return true;
}