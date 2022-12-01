#include "Camera.h"

Camera::Camera(const glm::vec3 _pos, const glm::vec3 _lookat, float _aspect, float _fov): pos(_pos),lookat(_lookat),aspect(_aspect),fov(_fov){
	float h = std::tan(fov / 2.f);
	float viewport_height = 2.f * h;
	float viewport_width = viewport_height * aspect;

	// Camera Matrix
	auto w = glm::normalize(pos - lookat);
	auto u = glm::normalize(glm::cross(up, w));
	auto v = glm::cross(w, u);

	cameraMatrix = {
		u.x,u.y,u.z,
		v.x,v.y,v.z,
		w.x,w.y,w.z
	};
}

Ray Camera::getRay(const glm::vec2& pixel,const glm::vec2& screenSpace) {
	// NDC Space, from 0 to 1
	glm::vec2 uv(static_cast<float>(pixel.x) / screenSpace.x, static_cast<float>(pixel.y) / screenSpace.y);
	// Screen Space, x:[-aspect,aspect],y:[-1,1]
	glm::vec2 screen((uv.x * 2.f - 1.f) * aspect, -(uv.y * 2.f - 1.f));
	// Camera Space
	float h = std::tan(fov / 2.f);
	glm::vec3 camera(screen.x * h, screen.y * h, -1.f);

	glm::vec3 dir = glm::normalize(cameraMatrix * camera);

	return Ray(pos, dir);
}