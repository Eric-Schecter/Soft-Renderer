#pragma once

#include <string>
#include <gl/glew.h>
#include <vector>
#include <glm/mat4x4.hpp>

#include "Vertex.h"
#include "Camera.h"

class Mesh
{
public:
	void setupModel(std::string path);
	void setupProgram(std::string vertexPath,std::string fragmentPath);
	void setupUniforms(Camera* camera);

	GLuint program;
	GLuint vao;

	GLuint pipeline = 0;
	GLuint vertProg = 0;
	GLuint fragProg = 0;

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	glm::mat4 modelMatrix = glm::mat4(1.f);

	GLint uModelMatrix;
	GLint uViewMatrix;
	GLint uProjectionMatrix;
};

