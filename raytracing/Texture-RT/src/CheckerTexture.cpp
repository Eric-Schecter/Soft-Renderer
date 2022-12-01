#include "CheckerTexture.h"

#include <math.h>

glm::vec3 CheckerTexture::sample(const glm::vec2& uv,const glm::vec3& p) const {
	float sines = sin(20.f*p.x) * sin(20.f*p.y) * sin(20.f*p.z);
	if (sines >0.f) {
		return odd->sample(uv,p);
	}
	else {
		return even->sample(uv,p);
	}
}