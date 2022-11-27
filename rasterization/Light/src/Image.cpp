#include "Image.h"

#include <stb_image.h>
#include <stdlib.h>

Image::Image(int width, int height, int bpp, unsigned char* data) :m_width(width), m_height(height), m_bpp(bpp), m_data(data) {};

Image::~Image() {
	stbi_image_free(m_data);
	m_data = nullptr;
}

// stb_image load image as RGB format
glm::vec4 Image::sample(const glm::vec2& uv) {
	if (this == nullptr) {
		return glm::vec4{ 0.f };
	}
	int x = uv.x * m_width;
	int y = uv.y * m_height;
	unsigned char* pixelOffset = m_data + (x + y * m_width) * m_bpp;
	double r = static_cast<int>(pixelOffset[0])/255.;
	double g = static_cast<int>(pixelOffset[1])/255.;
	double b = static_cast<int>(pixelOffset[2])/255.;
	double a = 1.;
	return glm::vec4(r,g,b,a);
}	