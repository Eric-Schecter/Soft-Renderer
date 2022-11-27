#pragma once

#include "Texture.h"

//todo: generate mipmap
class ColorTexture : public Texture
{
public:
	ColorTexture(int width, int height, unsigned char* data) :Texture(width, height) { m_data = data; };
	~ColorTexture() override;
	ColorTexture(const ColorTexture&);
	ColorTexture& operator=(const ColorTexture&);

	glm::vec4 sample(const glm::vec2& uv) const override;

	unsigned char* m_data = nullptr;
};
