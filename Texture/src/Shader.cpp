#include "Shader.h"

glm::vec4 Shader::vertex(const glm::vec3& a_position,const glm::vec2& a_uv,
	const glm::vec3& a_tangent, const glm::vec3& a_normal, const glm::vec3& a_biTangent,int index) {
	v_uv[index] = a_uv;

	glm::vec3 T = glm::normalize(u_normalMatrix * a_tangent);
	glm::vec3 N = glm::normalize(u_normalMatrix * a_normal);
	glm::vec3 B = glm::normalize(u_normalMatrix * a_biTangent);
	v_TBN[index] = glm::transpose(glm::mat3(T, B, N));

	return u_projection * u_view * u_model * glm::vec4(a_position, 1.f);
}

bool Shader::fragment(const glm::vec3& bar, glm::vec4& gl_fragment) {
	glm::vec2 uv = bar.x * v_uv[0] + bar.y * v_uv[1] + bar.z * v_uv[2];
	glm::mat3 TBN = bar.x * v_TBN[0] + bar.y * v_TBN[1] + bar.z * v_TBN[2];

	glm::vec3 color = glm::vec3(u_diffuse->sample(uv));

	gl_fragment = glm::vec4(color,1.f);

	return false;
}