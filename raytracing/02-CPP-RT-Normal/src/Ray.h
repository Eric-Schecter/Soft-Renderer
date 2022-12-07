#pragma once

#include <glm/vec3.hpp>

class Ray
{
public:
	Ray(const glm::vec3& _origin,const glm::vec3& _direction):origin(_origin),direction(_direction){};

	glm::vec3 at(float t) const;

	glm::vec3 origin;
	glm::vec3 direction;
};

