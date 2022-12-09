#pragma once

#include "Renderer.h"

#include <array>

class TriangleRendererDSA : public Renderer
{
public:
	TriangleRendererDSA();

	void render(int width, int height) const override;

private:
	GLuint vao;
	GLuint program;

	std::array<GLfloat, 18> vertices = {
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	std::array<GLint, 3> indices = {
		0,1,2
	};
};

