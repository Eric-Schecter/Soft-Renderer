#pragma once

#include <glm/vec3.hpp>
#include <gl/glew.h>
#include <vector>

#include "Vertex.h"
#include "Mesh.h"
#include "Camera.h"

class Renderer
{
public:
	Renderer();

	virtual void render(const Mesh& mesh,Camera* camera) const = 0;
};

