#pragma once

#include <cstdint>
#include <glm/vec4.hpp>
#include <vector>

#include "Vertex.h"

class Renderer
{
public:
	void setFramebuffer(uint32_t* pixels,int width,int height);
	void clear();
	void render(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const glm::vec4& color);

private:
	void setPixel(int x, int y, const glm::vec4& color);
	uint32_t touint32(const glm::vec4& color) const;
	void drawTriangle( glm::vec3& v1,  glm::vec3& v2,  glm::vec3& v3, const glm::vec4& color);

	uint32_t*  m_framebuffer;
	float* m_zbuffer;
	int m_width;
	int m_height;
};

