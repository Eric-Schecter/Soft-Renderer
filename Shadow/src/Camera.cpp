#include "Camera.h"

#include <glm/gtx/transform.hpp>

Camera* Camera::setProjection(float fov,float aspect,float near,float far) {
	//projection = glm::perspective(fov, aspect, near, far);

	// https://zhuanlan.zhihu.com/p/122411512
	const float tanHalfFovy = std::tan(fov * 0.5f);
	float f_n = far - near;
	projection = {
		1.0f / (aspect * tanHalfFovy), 0.0f,				0.0f,					    0.0f,
		0.0f,						   1.0f / tanHalfFovy,  0.0f,					    0.0f,
		0.0f,						   0.0f,			    -(far + near) / f_n,	    -1.0f,
		0.0f,						   0.0f,				-2.0f * near * far / f_n,	0.0f,
	};

	return this;
}

Camera* Camera::setOrthographic(float left, float right, float bottom, float top,float near,float far) {
	//projection = glm::ortho(left, right, bottom, top, near, far);

	// https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/orthographic-projection-matrix	
	projection = {
		2.f / (right - left)			, 0.f							  ,	0.f						    , 0.f,
		0.f								, 2.f / (top - bottom)			  , 0.f							, 0.f,
		0.f								, 0.f							  ,	-2.f / (far - near)			, 0.f,
		-(right + left) / (right - left), -(top + bottom) / (top - bottom),	-(far + near) / (far - near), 1.f,
	};

	return this;
}

Camera* Camera::setView(const glm::vec3& focus) {
	//view = glm::lookAt(pos, focus, up);	

	glm::vec3 zAxis = glm::normalize(pos - focus); // right handled, look at -z
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