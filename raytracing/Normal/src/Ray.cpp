#include "Ray.h"

glm::vec3 Ray::at(float t) const {
	return origin + t * direction;
}