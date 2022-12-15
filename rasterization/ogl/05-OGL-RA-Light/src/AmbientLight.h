#pragma once

#include <gl/glew.h>
#include <glm/vec4.hpp>

struct AmbientLight {
	glm::vec4 color;
	GLfloat intensity;
};