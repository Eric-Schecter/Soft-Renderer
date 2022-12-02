#pragma once

#include "Material.h"

class Dielectrics : public Material
{
public:
	Dielectrics(float _ir):ir(_ir){}

	bool scatter(const Ray& ray, const Record& record, Ray& scatteredRay, glm::vec3& attenuation) override;

private:
	//Use Schlick's approximation 
	float reflectance(float cosine, float ref_idx);

	float ir;
};

