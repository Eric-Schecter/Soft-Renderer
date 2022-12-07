#include "Ray.h"

glm::vec3 Ray::at(float t) {
	return origin + t * direction;
}