#pragma once

#include <string>
#include <gl/glew.h>
#include <assimp/scene.h>

class Texture {
public:
	Texture(std::string _name, aiTextureType _type) : name(_name), type(_type) {}

	void load(const std::string& path);

	GLuint id = 0;
	aiTextureType type;
	std::string name;
	GLint location = 0;
};