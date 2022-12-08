#pragma once

#include "Renderer.h"

class TriangleRendererDSA : public Renderer
{
public:
	TriangleRendererDSA();

	void render(int width, int height) const override;

private:
	GLuint vao;
	GLuint program;
};

