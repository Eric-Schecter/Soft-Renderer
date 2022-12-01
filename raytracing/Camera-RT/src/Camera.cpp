#include "Camera.h"

#include "utils.h"

Camera::Camera(const glm::vec3 _pos, const glm::vec3 _lookat, float _aspect, float _fov, float aperture, float focus_dist)
	: pos(_pos),lookat(_lookat),aspect(_aspect),fov(_fov), lenRadius(aperture/2.f){
	float h = std::tan(fov / 2.f);
	float viewport_height = 2.f * h;
	float viewport_width = viewport_height * aspect;

	// Camera Matrix
	auto w = glm::normalize(pos - lookat);
	u = glm::normalize(glm::cross(up, w));
	v = glm::cross(w, u);

	cameraMatrix = glm::mat3{
		u.x,u.y,u.z,
		v.x,v.y,v.z,
		w.x,w.y,w.z
	} * focus_dist;
}

Ray Camera::getRay(const glm::vec2& pixel,const glm::vec2& screenSpace) {
	// NDC Space, from 0 to 1
	glm::vec2 ndc(static_cast<float>(pixel.x) / screenSpace.x, static_cast<float>(pixel.y) / screenSpace.y);
	// Screen Space, x:[-aspect,aspect],y:[-1,1]
	glm::vec2 screen((ndc.x * 2.f - 1.f) * aspect, -(ndc.y * 2.f - 1.f));
	// Camera Space
	float h = std::tan(fov / 2.f);
	glm::vec3 camera(screen.x * h, screen.y * h, -1.f);
	// defocus
	glm::vec3 randomOffset = lenRadius * utils::getRandomVecInUnitCircle();
	glm::vec3 offset = u * randomOffset.x + v * randomOffset.y;

	glm::vec3 dir = glm::normalize(cameraMatrix * camera - offset);

	return Ray(pos + offset, dir);
}