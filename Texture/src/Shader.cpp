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

bool Shader::fragment(const glm::vec3& bar, glm::vec3& gl_fragment) {
	glm::vec2 uv = bar.x * v_uv[0] + bar.y * v_uv[1] + bar.z * v_uv[2];
	glm::mat3 TBN = bar.x * v_TBN[0] + bar.y * v_TBN[1] + bar.z * v_TBN[2];

	glm::vec3 color = glm::vec3(u_diffuse->sample(uv));
	glm::vec3 normal = glm::vec3(u_normal->sample(uv));
	normal = glm::normalize(normal * 2.f - glm::vec3(1.));
	normal = glm::normalize(TBN * normal);

	gl_fragment = glm::clamp(glm::dot(normal,u_light),0.f,1.f) * color;
	return false;
}