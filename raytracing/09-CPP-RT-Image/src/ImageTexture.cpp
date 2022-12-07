#include "ImageTexture.h"

#include <glm/geometric.hpp>
#include <stb_image.h>
#include <algorithm>
#include <iostream>

ImageTexture::ImageTexture(const char* filename) {
	data = stbi_load(filename, &width, &height, &bpp, 0);
	if (!data) {
		std::cerr << "Count not load texture " << filename << "\n";
	}
}

ImageTexture::~ImageTexture() {
	delete data;
}

glm::vec3 ImageTexture::sample(const glm::vec2& uv, const glm::vec3& p) const {
	if (data == nullptr) {
		return glm::vec3(1.f, 0.f, 1.f);
	}
	float u = glm::clamp(uv.x, 0.f, 1.f);
	float v = 1.f - glm::clamp(uv.y, 0.f, 1.f);
	
	int i = static_cast<int>(u * width);
	int j = static_cast<int>(v * height);
	i = std::min(i, width - 1);
	j = std::min(j, height - 1);

	auto pixel = data + j * bpp * width + i * bpp;

	return glm::vec3(pixel[0]/255.f, pixel[1] / 255.f, pixel[2] / 255.f);
}