#pragma once

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "Image.h"

class Shader {
public:
	glm::vec4 vertex( // default output position for interplation
		const glm::vec3& position,
		const glm::vec3& normal,
		const glm::vec2& uv,
		int index
	);
	bool fragment(const glm::vec3& bar,glm::vec3& gl_fragment);

	glm::mat4 u_projection;
	glm::mat4 u_view;
	glm::mat4 u_model;
	glm::vec3 u_light;
	Image* u_diffuse;

	glm::vec4 gl_position;
	glm::vec3 v_normal[3];
	glm::vec2 v_uv[3];
};