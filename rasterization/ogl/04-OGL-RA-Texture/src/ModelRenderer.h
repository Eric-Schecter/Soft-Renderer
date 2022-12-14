#pragma once

#include "Renderer.h"
#include "Mesh.h"
#include "Camera.h"

#include <array>

class ModelRenderer : public Renderer
{
public:
	void render(const Mesh& mesh, Camera* camera) const override;
};

