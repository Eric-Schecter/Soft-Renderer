#pragma once

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "Image.h"

class Shader {
public:
	glm::vec4 vertex( // default output position for interplation
		const glm::vec3& a_position,
		const glm::vec2& a_uv,
		const glm::vec3& a_tangent, 
		const glm::vec3& a_normal, 
		const glm::vec3& a_biTangent,
		int index
	);
	bool fragment(const glm::vec3& bar,glm::vec3& gl_fragment);

	glm::mat4 u_projection;
	glm::mat4 u_view;
	glm::mat4 u_model;
	glm::vec3 u_light;
	glm::mat3 u_normalMatrix;
	Image* u_diffuse;
	Image* u_normal;

	glm::vec4 gl_position;
	glm::vec2 v_uv[3];
	glm::mat3 v_TBN[3];
};