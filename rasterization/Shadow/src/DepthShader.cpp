#include "DepthShader.h"

glm::vec4 DepthShader::vertex(int vertexIndex, int index) {
	auto a_position = vertices[vertexIndex].position;
	return u_projection * u_view * u_model * glm::vec4(a_position, 1.f);
};

bool DepthShader::fragment(const glm::vec3& bar, glm::vec4& gl_fragment, const std::vector<glm::vec4>& gl_FragCoord) {
	float zArray[3] = { gl_FragCoord[0].z, gl_FragCoord[1].z, gl_FragCoord[2].z };
	float depth = interpolate(bar, zArray);
	float ndc = depth * 2.f - 1.f;
	const float u_near = 0.1f;
	const float u_far = 100.f;
	float z = glm::clamp((2.f * u_near ) / (u_far + u_near - ndc * (u_far - u_near)),0.f,1.f);
	gl_fragment = glm::vec4(depth, depth, depth,1.f);
	return false;
};