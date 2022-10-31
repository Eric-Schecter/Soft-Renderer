#pragma once

#include <glm\vec3.hpp>
#include <glm\vec2.hpp>

class Image {
public:
	Image(int width, int height, int bpp, unsigned char* data);
	~Image();

	glm::vec3 sample(const glm::vec2& uv);

private:
	int m_width = 0;
	int m_height = 0;
	int m_bpp = 0;
	unsigned char* m_data = nullptr;
};