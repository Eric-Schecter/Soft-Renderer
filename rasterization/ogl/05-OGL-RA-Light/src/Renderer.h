#pragma once

#include "Vertex.h"
#include "Mesh.h"
#include "Camera.h"
#include "Lights.h"

class Renderer
{
public:
	Renderer();

	virtual void render(const Mesh& mesh,Camera* camera, const Lights& lights) const = 0;
};

