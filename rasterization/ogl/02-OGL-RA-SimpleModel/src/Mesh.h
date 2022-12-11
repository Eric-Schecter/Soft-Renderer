#pragma once

#include <string>
#include <gl/glew.h>
#include <vector>

#include "Vertex.h"

class Mesh
{
public:
	void loadModel(std::string path);
	void setupProgram(std::string vertexPath,std::string fragmentPath);

	GLuint program;
	GLuint vao;
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
};

