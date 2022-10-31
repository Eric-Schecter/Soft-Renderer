#include "Shader.h"

glm::vec4 Shader::vertex(const glm::vec3& position,const glm::vec2& uv,int index) {
	v_uv[index] = uv;
	return u_projection * u_view * u_model * glm::vec4(position, 1.f);
}

bool Shader::fragment(const glm::vec3& bar, glm::vec3& gl_fragment) {
	glm::vec2 uv = bar.x * v_uv[0] + bar.y * v_uv[1] + bar.z * v_uv[2];
	
	glm::vec3 color = u_diffuse->sample(uv);
	glm::vec3 normal = u_normal->sample(uv);

	gl_fragment = glm::clamp(glm::dot(normal,u_light),0.f,1.f) * color;
	return false;
}