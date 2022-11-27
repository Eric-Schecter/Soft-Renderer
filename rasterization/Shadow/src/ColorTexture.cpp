#include "ColorTexture.h"

#include <stb_image.h>

ColorTexture::~ColorTexture() {
	delete[] m_data;
	m_data = nullptr;
}

ColorTexture::ColorTexture(const ColorTexture& texture) :Texture(texture.m_width, texture.m_height) {
	m_data = texture.m_data;
}

ColorTexture& ColorTexture::operator=(const ColorTexture& texture)
{
	m_width = texture.m_width;
	m_height = texture.m_height;
	m_data = texture.m_data;
	return *this;
}

glm::vec4 ColorTexture::sample(const glm::vec2& uv) const {
	if (this == nullptr) {
		return glm::vec4{ 0.f };
	}
	int x = static_cast<int>(uv.x * m_width);
	int y = static_cast<int>(uv.y * m_height);
	unsigned char* pixelOffset = m_data + (x + y * m_width) * 3;
	float r = static_cast<int>(pixelOffset[0]) / 255.f;
	float g = static_cast<int>(pixelOffset[1]) / 255.f;
	float b = static_cast<int>(pixelOffset[2]) / 255.f;
	float a = 1.f;
	return glm::vec4(r, g, b, a);
}