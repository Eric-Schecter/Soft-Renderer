#include "Shader.h"

glm::vec4 Shader::vertex(const glm::vec3& position,const glm::vec3& normal,int index) {
	v_normal[index] = normal;
	return u_projection * u_view * u_model * glm::vec4(position, 1.f);
}

bool Shader::fragment(const glm::vec3& bar, glm::vec3& color) {
	glm::vec3 normal = bar.x * v_normal[0] + bar.y * v_normal[1] + bar.z * v_normal[2];
	color = glm::clamp(glm::dot(normal,u_light),0.f,1.f) * color;
	return false;
}