#pragma once

#include <string>

class ImageLoader
{
public:
	ImageLoader(const std::string& path);
	~ImageLoader();

	unsigned char* image_data = nullptr;
	int width = 0;
	int height = 0;
	int bpp = 0;
};

