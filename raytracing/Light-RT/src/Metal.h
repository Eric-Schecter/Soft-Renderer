#pragma once

#include "Material.h"
#include "Texture.h"

#include <memory>

class Metal:public Material
{
public:
	Metal(const std::shared_ptr<Texture>& _texture,float _fuzz = 0.f):texture(_texture),fuzz(std::min(_fuzz,1.f)) {};

	bool scatter(const Ray& ray, const Record& record, Ray& scatteredRay, glm::vec3& attenuation) const override;

	std::shared_ptr<Texture> texture;
	float fuzz;
};

