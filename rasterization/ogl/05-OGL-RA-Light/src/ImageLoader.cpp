#include "ImageLoader.h"

#include <stb_image.h>

ImageLoader::ImageLoader(const std::string& path) {
	image_data = stbi_load(path.c_str(), &width, &height, &bpp, 0);
}
ImageLoader::~ImageLoader() {
	stbi_image_free(image_data);
}