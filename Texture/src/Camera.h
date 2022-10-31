#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class Camera 
{
public:
	Camera* setView(const glm::vec3& focus);
	Camera* setViewport(int x,int y,int width,int height);
	Camera* setProjection(float fov, float aspect, float near, float far);

	glm::mat4 projection = glm::mat4(1.f);
	glm::mat4 view = glm::mat4(1.f);
	glm::mat4 viewport = glm::mat4(1.f);

	glm::vec3 pos = glm::vec3(0,0,0);
	glm::vec3 up = glm::vec3(0,1.f,0);
};
