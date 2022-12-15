#pragma once

#include "Renderer.h"
#include "Mesh.h"
#include "Camera.h"
#include "Lights.h"

class ModelRenderer : public Renderer
{
public:
	void render(const Mesh& mesh, Camera* camera, const Lights& lights) const override;

private:
	void updateUniformsBasic(const Mesh& mesh, Camera* camera, const Lights& lights) const;
};

