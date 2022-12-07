#pragma once

#include <glm/vec3.hpp>

class Ray
{
public:
	Ray() = default;
	Ray(const glm::vec3& _origin, const glm::vec3& _direction);

	inline glm::vec3 at(float t) const {
		return origin + t * direction;
	}

	glm::vec3 origin;
	glm::vec3 direction;
	glm::vec3 invdir;
	int sign[3];
};

