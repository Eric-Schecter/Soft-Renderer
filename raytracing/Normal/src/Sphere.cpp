#include "Sphere.h"

#include <glm/glm.hpp>

bool Sphere::hit(const Ray& ray, Record& record) {
	glm::vec3 oc = ray.origin - center;
	float a = glm::dot(ray.direction, ray.direction);
	float hb = glm::dot(oc, ray.direction);
	float c = glm::dot(oc, oc) - radius * radius;
	float discriminant = hb * hb - a * c;
	if (discriminant < 0.f) {
		return false;
	}

	float min = 0.f; 
	float max = record.t;
	float root = (-hb + std::sqrt(discriminant)) / a;
	if (root<min || root>max) {
		return false;
	}
	root = (-hb - std::sqrt(discriminant)) / a;
	if (root<min || root>max) {
		return false;
	}

	record.isHit = true;
	record.t = root;
	record.pos = ray.at(root);
	record.normal = (record.pos - center) / radius;
	return true;
}