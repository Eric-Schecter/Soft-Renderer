#pragma once

#include "Shader.h"
#include "Vertex.h"

#include <glm/mat4x4.hpp>
#include <vector>

class DepthShader : public Shader
{
public:
	glm::vec4 vertex(int vertexIndex, int index) override;
	bool fragment(const glm::vec3& bar, glm::vec4& gl_fragment, const std::vector<glm::vec4>& gl_FragCoord) override;

	// uniforms
	glm::mat4 u_projection = glm::mat4(1.f);
	glm::mat4 u_view = glm::mat4(1.f);
	glm::mat4 u_model = glm::mat4(1.f);

	// attributes
	std::vector<Vertex> vertices;

	// output
	glm::vec4 gl_position = glm::vec4(0.f);
};

