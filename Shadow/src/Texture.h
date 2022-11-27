#pragma once

#include <glm\vec4.hpp>
#include <glm\vec2.hpp>

class Texture {
public:
	Texture(int width, int height);
	virtual ~Texture() {};

	virtual glm::vec4 sample(const glm::vec2& uv) const = 0;

protected:
	int m_width = 0;
	int m_height = 0;
};
