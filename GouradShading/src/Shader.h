#pragma once

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

class Shader {
public:
	glm::vec4 vertex( // default output position for interplation
		const glm::vec3& position, 
		const glm::vec3& normal, 
		int index
	);
	bool fragment(const glm::vec3& bar,glm::vec3& color);

	glm::mat4 u_projection;
	glm::mat4 u_view;
	glm::mat4 u_model;
	glm::vec3 u_light;

	glm::vec4 gl_position;
	glm::vec3 v_intensity;
};