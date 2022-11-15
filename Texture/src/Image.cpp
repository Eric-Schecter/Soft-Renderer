#include "Image.h"

#include <stb_image.h>
#include <stdlib.h>

Image::Image(int width, int height, int bpp, unsigned char* data) :m_width(width), m_height(height), m_bpp(bpp), m_data(data) {};

Image::~Image() {
	stbi_image_free(m_data);
	m_data = nullptr;
}

glm::vec4 Image::sample(const glm::vec2& uv) {
	int x = uv.x * m_width;
	int y = uv.y * m_height;
	unsigned char* pixelOffset = m_data + (x + y * m_width) * m_bpp;
	float r = static_cast<float>(pixelOffset[0])/255.f;
	float g = static_cast<float>(pixelOffset[1])/255.f;
	float b = static_cast<float>(pixelOffset[2])/255.f;
	float a = 1.f;
	return glm::vec4(r,g,b,a);
}	