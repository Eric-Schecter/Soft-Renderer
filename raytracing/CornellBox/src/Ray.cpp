#include "Ray.h"

Ray::Ray(const glm::vec3& _origin, const glm::vec3& _direction) :origin(_origin), direction(_direction) {
	invdir = 1.f / direction;
	sign[0] = (invdir.x < 0);
	sign[1] = (invdir.y < 0);
	sign[2] = (invdir.z < 0);
};