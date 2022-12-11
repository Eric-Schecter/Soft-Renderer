#pragma once

#include "Renderer.h"
#include "Mesh.h"

#include <array>

class ModelRenderer : public Renderer
{
public:
	void render(int width, int height, const Mesh& mesh) const override;
};

