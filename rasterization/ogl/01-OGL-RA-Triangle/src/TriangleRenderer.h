#pragma once

#include "Renderer.h"

class TriangleRenderer : public Renderer
{
public:
	TriangleRenderer();

	void render(int width, int height) const override;

private:
	GLuint vao;
	GLuint program;
};

