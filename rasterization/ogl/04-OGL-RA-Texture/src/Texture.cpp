#include "Texture.h"

#include "ImageLoader.h"

void Texture::load(const std::string& path) {
	ImageLoader imageLoader(path);
	int bpp = imageLoader.bpp;
	int width = imageLoader.width;
	int height = imageLoader.height;
	unsigned char* image_data = imageLoader.image_data;

	// init 
	glGenTextures(1, &id);
	// bind
	glBindTexture(GL_TEXTURE_2D, id);
	// set parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// set format
	switch (bpp)
	{
	case 1: {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R, width, height, 0, GL_R, GL_UNSIGNED_BYTE, image_data);
		break;
	}
	case 3: {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
		break;
	}
	case 4: {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
		break;
	}
	default:
		break;
	}
	// generate mipmap
	glGenerateMipmap(GL_TEXTURE_2D);
}