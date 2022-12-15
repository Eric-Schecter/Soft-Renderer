#pragma once

#include <gl/glew.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct PointLight {
	glm::vec4 color;
	glm::vec3 pos;
	GLfloat constant;
	GLfloat intensity;
	GLfloat linear;
	GLfloat quadratic;
};