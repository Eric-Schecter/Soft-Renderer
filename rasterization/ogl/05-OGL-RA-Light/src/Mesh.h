#pragma once

#include <string>
#include <gl/glew.h>
#include <vector>
#include <glm/mat4x4.hpp>
#include <unordered_map>

#include "Vertex.h"
#include "Camera.h"
#include "Texture.h"
#include "Material.h"
#include "Lights.h"

struct UniformInfo
{
	GLint location;
	GLsizei count;
};

class Mesh
{
public:
	void setupModel(std::string path);
	void setupProgram(std::string vertexPath,std::string fragmentPath);
	void setupUniforms(Camera* camera, const Lights& lights);
	void updateUniformsBasic(Camera* camera, const Lights& lights) const;

	GLuint vao;

	// single program
	GLuint program;

	// spread programs
	GLuint pipeline = 0;
	GLuint vertProg = 0;
	GLuint fragProg = 0;

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	glm::mat4 modelMatrix = glm::mat4(1.f);

	// unidforms
	GLint uModelMatrix;
	GLint uViewMatrix;
	GLint uProjectionMatrix;
	GLint uNormalMatrix;

	std::unordered_map<std::string, UniformInfo> uniforms;

	std::unordered_map<std::string, UniformInfo> uniformsVert;
	std::unordered_map<std::string, UniformInfo> uniformsFrag;

	// textures
	std::vector<Texture> textures;
	Material uMaterial;
};

