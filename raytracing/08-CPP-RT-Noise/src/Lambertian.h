#pragma once

#include "Material.h"
#include "Record.h"
#include "Ray.h"
#include "Texture.h"

#include <glm/vec3.hpp>
#include <memory>

class Lambertian:public Material
{
public:
	Lambertian(const std::shared_ptr<Texture>& _texture) :texture(_texture) {};

	bool scatter(const Ray& ray, const Record& record, Ray& scatteredRay, glm::vec3& attenuation) override;

	std::shared_ptr<Texture> texture;
};

