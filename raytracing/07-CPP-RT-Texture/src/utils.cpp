#include "utils.h"

#include <cstdlib>
#include <glm/geometric.hpp>

float utils::random(float min, float max) {
	return std::rand() * (max - min) / RAND_MAX + min;
}

glm::vec3 utils::getRandomVecInUnitCircle() {
	while (true) {
		using namespace utils;
		glm::vec3 vec = glm::vec3(random(-1.f, 1.f), random(-1.f, 1.f), 0.f);
		if (glm::length(vec) < 1.f) {
			return vec;
		}
	}
}

glm::vec3 utils::getRandomUnitVec() {
	while (true) {
		using namespace utils;
		glm::vec3 vec = glm::vec3(random(-1.f, 1.f), random(-1.f, 1.f), random(-1.f, 1.f));
		if (glm::length(vec) < 1.f) {
			return glm::normalize(vec);
		}
	}
}

glm::vec3 utils::getRandomUnitVecByNormal(const glm::vec3& normal) {
	while (true) {
		using namespace utils;
		glm::vec3 vec = glm::vec3(random(-1.f, 1.f), random(-1.f, 1.f), random(-1.f, 1.f));
		if (glm::length(vec) < 1.f && glm::dot(vec, normal) >= 0.f) {
			return glm::normalize(vec);
		}
	}
}