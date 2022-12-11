#pragma once

#include <glm/vec3.hpp>
#include <gl/glew.h>
#include <vector>

#include "Vertex.h"
#include "Mesh.h"

class Renderer
{
public:
	virtual void render(int width, int height, const Mesh& mesh) const = 0;
};

