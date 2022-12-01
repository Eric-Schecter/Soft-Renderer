#include "Material.h"

#include <glm/geometric.hpp>

float Material::random(float min, float max) {
	return std::rand() * (max - min) / RAND_MAX + min;
}

glm::vec3 Material::getRandomUnitVec() {
	while (true) {
		glm::vec3 vec = glm::vec3(random(-1.f, 1.f), random(-1.f, 1.f), random(-1.f, 1.f));
		if (glm::length(vec) < 1.f) {
			return glm::normalize(vec);
		}
	}
}

glm::vec3 Material::getRandomUnitVecByNormal(const glm::vec3& normal) {
	while (true) {
		glm::vec3 vec = glm::vec3(random(-1.f, 1.f), random(-1.f, 1.f), random(-1.f, 1.f));
		if (glm::length(vec) < 1.f && glm::dot(vec,normal)>=0.f) {
			return glm::normalize(vec);
		}
	}
}