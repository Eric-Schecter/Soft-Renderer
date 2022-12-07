#pragma once

#include "Texture.h"

class ColorTexture : public Texture
{
public:
	ColorTexture(const glm::vec3& _color):color(_color){}

	inline glm::vec3 sample(const glm::vec2& uv, const glm::vec3& p) const override {
		return color;
	}

private:
	glm::vec3 color;
};

