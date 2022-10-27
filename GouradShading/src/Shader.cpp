#include "Shader.h"

glm::vec4 Shader::vertex(const glm::vec3& position,const glm::vec3& normal,int index) {
	v_intensity[index] = glm::clamp(glm::dot(normal, u_light),0.f,1.f); // compute light component in vertex shader
	return u_projection * u_view * u_model * glm::vec4(position, 1.f);
}

bool Shader::fragment(const glm::vec3& bar, glm::vec3& color) {
	color = (bar.x * v_intensity.x + bar.y * v_intensity.y + bar.z * v_intensity.z) * color;
	return false;
}