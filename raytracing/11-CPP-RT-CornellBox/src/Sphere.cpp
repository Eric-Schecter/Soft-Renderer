#include "Sphere.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

glm::vec2 Sphere::getUV(const glm::vec3& p) {
	float pi = glm::pi<float>();
	float theta = std::acos(-p.y);
	float phi = std::atan2(-p.z, p.x) + pi;
	float u = phi / (2.f * pi);
	float v = theta / pi;
	return glm::vec2(u,v);
}

bool Sphere::hit(const Ray& ray, Record& record) {
	glm::vec3 oc = ray.origin - center;
	float a = glm::dot(ray.direction, ray.direction);
	float hb = glm::dot(oc, ray.direction);
	float c = glm::dot(oc, oc) - radius * radius;
	float discriminant = hb * hb - a * c;
	if (discriminant < 0.f) {
		return false;
	}

	float min = 0.001f; 
	float max = record.t;
	float sqrtd = std::sqrt(discriminant);
	float root = (-hb - sqrtd) / a;
	if (root<min || root>max) {
		root = (-hb + sqrtd) / a;
		if (root<min || root>max) {
			return false;
		}
	}
	if (root > record.t) {
		return false;
	}

	record.isHit = true;
	record.t = root;
	record.pos = ray.at(root);
	glm::vec3 normal = glm::normalize((record.pos - center) / radius);
	record.isFrontFace = checkFrontFace(ray.direction, normal);
	record.normal = record.isFrontFace ? normal : -normal;
	record.material = material;
	record.uv = getUV(normal);
	return true;
}