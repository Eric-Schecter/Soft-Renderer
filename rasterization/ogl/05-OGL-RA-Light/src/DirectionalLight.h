#pragma once

#include <gl/glew.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct DirectionalLight {
	glm::vec3 direction;
	glm::vec4 color;
	GLfloat intensity;
};