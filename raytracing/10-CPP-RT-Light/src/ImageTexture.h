#pragma once

#include "Texture.h"

class ImageTexture : public Texture
{
public:
	ImageTexture(const char* filename);
	~ImageTexture();

	glm::vec3 sample(const glm::vec2& uv, const glm::vec3& p) const override;

private:
	unsigned char* data;
	int width;
	int height;
	int bpp;
};

