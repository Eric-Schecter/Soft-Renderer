#pragma once

#include <cstdint>
#include <glm/vec4.hpp>

#include "Vertex.h"

class Renderer
{
public:
	void setFramebuffer(uint32_t* pixels,int width,int height);
	void clear();
	void render(Vertex& v1,Vertex& v2, Vertex& v3,const glm::vec4& color);
	void renderByPixel();

private:
	void setPixel(int x, int y, const glm::vec4& color);
	uint32_t touint32(const glm::vec4& color) const;

	uint32_t*  m_framebuffer;
	int m_width;
	int m_height;
};

