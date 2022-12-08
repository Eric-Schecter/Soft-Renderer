#pragma once

#include <glm/vec3.hpp>
#include <gl/glew.h>

class Renderer
{
public:
	virtual void render(int width, int height) const = 0;
};

