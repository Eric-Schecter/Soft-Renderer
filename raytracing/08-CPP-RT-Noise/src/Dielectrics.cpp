#include "Dielectrics.h"

#include <glm/geometric.hpp>

float Dielectrics::reflectance(float cosine, float ref_idx) {
	auto r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow((1 - cosine), 5);
}

bool Dielectrics::scatter(const Ray& ray, const Record& record, Ray& scatteredRay, glm::vec3& attenuation) {
	float refractionRatio = record.isFrontFace ? (1.f / ir) : ir;

	// TIR: Total internal reflection
	float cos_theta = glm::dot(-ray.direction,record.normal);
	float sin_theta = std::sqrt(1.f - cos_theta * cos_theta);
	bool TIR = refractionRatio * sin_theta > 1.f;
	bool SchlickReflection = reflectance(cos_theta, refractionRatio) > 1.5f;
	glm::vec3 direction;
	if (TIR || SchlickReflection){
		direction = glm::reflect(ray.direction, record.normal);
	}
	else {
		direction = glm::refract(ray.direction, record.normal, refractionRatio);
	}

	scatteredRay = Ray(record.pos, direction);
	attenuation = glm::vec3(1.f);
	return true;
}