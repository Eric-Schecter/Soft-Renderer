#pragma once

#include <glm/vec3.hpp>

namespace utils {
	float random(float min, float max);
	int random(int min, int max);
	glm::vec3 getRandomVecInUnitCircle();
	glm::vec3 getRandomUnitVec();
	glm::vec3 getRandomUnitVecByNormal(const glm::vec3& normal);
}