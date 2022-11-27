#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class Camera 
{
public:
	Camera* setView(const glm::vec3& focus);
	Camera* setProjection(float fov, float aspect, float near, float far);
	Camera* setOrthographic(float left, float right, float bottom, float top,float near,float far);
	inline glm::mat4 getMVP() { return projection * view * model; };
	inline glm::mat4 getVP() { return projection * view; };

	glm::mat4 projection = glm::mat4(1.f);
	glm::mat4 view = glm::mat4(1.f);
	glm::mat4 model = glm::mat4(1.f);

	glm::vec3 pos = glm::vec3(0,0,0);
	glm::vec3 up = glm::vec3(0,1.f,0);
};
