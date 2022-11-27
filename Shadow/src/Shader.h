#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>

class Shader {
public:
	virtual ~Shader() {}

	virtual glm::vec4 vertex(int vertexIndex, int index) = 0;
	virtual bool fragment(const glm::vec3& bar,glm::vec4& gl_fragment, const std::vector<glm::vec4>& gl_FragCoord) = 0;

	template <typename T> T interpolate(const glm::vec3& bary, T x[]) {
		return bary.x * x[0] + bary.y * x[1] + bary.z * x[2];
	}
};