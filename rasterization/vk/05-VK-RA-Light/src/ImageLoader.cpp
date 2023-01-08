#include "ImageLoader.h"

#include <stb_image.h>

ImageLoader::ImageLoader(const std::string& path) {
	image_data = stbi_load(path.c_str(), &width, &height, &bpp, STBI_rgb_alpha);
}
ImageLoader::~ImageLoader() {
	stbi_image_free(image_data);
}