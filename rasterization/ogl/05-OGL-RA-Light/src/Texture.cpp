#include "Texture.h"

#include "ImageLoader.h"

void Texture::load(const std::string& path) {
	ImageLoader imageLoader(path);
	int bpp = imageLoader.bpp;
	int width = imageLoader.width;
	int height = imageLoader.height;
	unsigned char* image_data = imageLoader.image_data;

	//// basic style
	//// init 
	//glGenTextures(1, &id);
	//// bind
	//glBindTexture(GL_TEXTURE_2D, id);
	//// set parameters
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// set format and data
	//switch (bpp)
	//{
	//case 1: {
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_R, width, height, 0, GL_R, GL_UNSIGNED_BYTE, image_data);
	//	break;
	//}
	//case 3: {
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
	//	break;
	//}
	//case 4: {
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	//	break;
	//}
	//default:
	//	break;
	//}
	
	// or can set format and data seperately
	// internal format and format https://www.cs.uregina.ca/Links/class-info/315/WWW/Lab5/InternalFormats_OGL_Core_3_2.html
	//switch (bpp)
	//{
	//case 1: {
	//	glTexStorage2D(GL_TEXTURE_2D,1, GL_R8, width, height);
	//	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_R, GL_UNSIGNED_BYTE, image_data);
	//	break;
	//}
	//case 3: {
	//	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, width, height);
	//	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image_data);
	//	break;
	//}
	//case 4: {
	//	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
	//	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	//	break;
	//}
	//default:
	//	break;
	//}

	// generate mipmap
	glGenerateMipmap(GL_TEXTURE_2D);

	// DSA style
	// glCreateTextures = glGenTextures + glBindTexture
	glCreateTextures(GL_TEXTURE_2D, 1, &id);
	// glTextureParameterx = glTexParameterx
	glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// set format and data
	switch (bpp)
	{
	case 1: {
		glTextureStorage2D(id, 1, GL_R8, width, height);
		glTextureSubImage2D(id, 0, 0, 0, width, height, GL_R, GL_UNSIGNED_BYTE, image_data);
		break;
	}
	case 3: {
		glTextureStorage2D(id, 1, GL_RGB8, width, height);
		glTextureSubImage2D(id, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image_data);
		break;
	}
	case 4: {
		glTextureStorage2D(id, 1, GL_RGBA8, width, height);
		glTextureSubImage2D(id, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
		break;
	}
	default:
		break;
	}

	// generate mipmap
	// glGenerateTextureMipmap = glGenerateMipmap;
	glGenerateTextureMipmap(id);
}