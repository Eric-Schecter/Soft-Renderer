#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

class Texture
{
public:
	virtual ~Texture() {};

	virtual glm::vec3 sample(const glm::vec2& uv, const glm::vec3& p) const = 0;
};

