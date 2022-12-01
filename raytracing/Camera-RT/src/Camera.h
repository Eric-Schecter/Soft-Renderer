#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>

#include "Ray.h"

class Camera
{
public:
	Camera() = default;
	Camera(const glm::vec3 pos,const glm::vec3 lookat,float aspect,float fov);

	Ray getRay(const glm::vec2& pixel,const glm::vec2& screenSpace);

	glm::vec3 pos = glm::vec3(0, 0, 1.f);
	glm::vec3 up = glm::vec3(0, 1.f, 0);
	glm::vec3 lookat = glm::vec3(0, 0, 0);
	float aspect = 1.f;
	float fov = glm::radians(60.f);

private:
	glm::vec3 horizontal;
	glm::vec3 vertical;
	glm::vec3 depth;
};

