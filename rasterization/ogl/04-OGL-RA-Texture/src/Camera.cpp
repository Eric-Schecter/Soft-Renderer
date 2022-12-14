#include "Camera.h"

#include <glm/gtx/transform.hpp>

Camera* Camera::setProjection(float fov, float aspect, float near, float far) {
	//projection = glm::perspective(fov, aspect, near, far);

	// https://zhuanlan.zhihu.com/p/122411512
	const float tanHalfFovy = std::tan(fov * 0.5f);
	float f_n = far - near;
	projection[0][0] = 1.0f / (aspect * tanHalfFovy); projection[0][1] = 0.0f;				projection[0][2] = 0.0f;					projection[0][3] = 0.0f;
	projection[1][0] = 0.0f;						  projection[1][1] = 1.0f / tanHalfFovy;  projection[1][2] = 0.0f;					projection[1][3] = 0.0f;
	projection[2][0] = 0.0f;						  projection[2][1] = 0.0f;			    projection[2][2] = -(far + near) / f_n;	    projection[2][3] = -1.0f;
	projection[3][0] = 0.0f;						  projection[3][1] = 0.0f;				projection[3][2] = -2.0f * near * far / f_n;	projection[3][3] = 0.0f;

	return this;
}

Camera* Camera::setView(const glm::vec3& focus) {
	//view = glm::lookAt(pos, focus, up);	

	glm::vec3 zAxis = glm::normalize(pos - focus);
	glm::vec3 xAxis = glm::normalize(glm::cross(up, zAxis));
	glm::vec3 yAxis = glm::normalize(glm::cross(zAxis, xAxis));

	view = {
		xAxis.x, yAxis.x, zAxis.x,  0.f,
		xAxis.y, yAxis.y, zAxis.y,  0.f,
		xAxis.z, yAxis.z, zAxis.z,  0.f,
		-glm::dot(xAxis, pos),-glm::dot(yAxis, pos),-glm::dot(zAxis, pos),1.f
	};

	return this;
}

Camera* Camera::setViewport(int x, int y, int width, int height) {
	viewport = {x,y,width,height};

	return this;
}