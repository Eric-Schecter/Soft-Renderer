#pragma once

#include <gl/glew.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct SpotLight {
	glm::vec4 color;
	glm::vec3 pos;
	glm::vec3 direction;
	GLfloat intensity;
	GLfloat cutOff;
	GLfloat outerCutOff;
};