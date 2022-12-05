#pragma once

#include "Material.h"
#include "Texture.h"
#include "ColorTexture.h"

#include <memory>

class Light : public Material
{
public:
	Light(std::shared_ptr<Texture> texture) :emit(texture) {};
	Light(const glm::vec3& color) :emit(std::make_shared<ColorTexture>(color)) {};

	inline glm::vec3 emitted(const glm::vec2& uv, const glm::vec3& p) const override{
		return emit->sample(uv,p);
	}
	inline bool scatter(const Ray& ray, const Record& record, Ray& scatteredRay, glm::vec3& attenuation) const override {
		return false;
	}

private:
	std::shared_ptr<Texture> emit;
};

