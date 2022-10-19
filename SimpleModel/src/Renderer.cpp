#include "Renderer.h"

#include <vcruntime_string.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <algorithm>

void Renderer::setFramebuffer(uint32_t* pixels,	int width,int height) {
	m_framebuffer = pixels;
	m_width = width;
	m_height = height;
}

void Renderer::clear() {
	memset(m_framebuffer,0,sizeof(uint32_t) * m_width * m_height);
}

uint32_t Renderer::touint32(const glm::vec4& color) const
{
	uint32_t r = (uint32_t)(color.r * 255.0f);
	uint32_t g = (uint32_t)(color.g * 255.0f);
	uint32_t b = (uint32_t)(color.b * 255.0f);
	uint32_t a = (uint32_t)(color.a * 255.0f);

	return (a << 24) | (r << 16) | (g << 8) | b;
}


void Renderer::setPixel(int x,int y, const glm::vec4& color) {
	// ignore pixels outside bounding
	if (x >= m_width || y >= m_height) {
		return;
	}

	m_framebuffer[y * m_width + x] = touint32(color);
}

glm::vec3 barycentric(const Vertex& v1, const Vertex& v2, const Vertex& v3, const Vertex& v4) {
	glm::vec3 a(v3.x-v1.x,v2.x-v1.x,v1.x-v4.x);
	glm::vec3 b(v3.y-v1.y,v2.y-v1.y,v1.y-v4.y);
	glm::vec3 c = glm::cross(a,b);

	// ignore degenerated triangle case
	if (c.z < 1) {
		return glm::vec3(-1, 1, 1);
	}

	return glm::vec3(1-(c.x+c.y)/c.z,c.y/c.z,c.x/c.z);
}

void Renderer::render(Vertex& v1, Vertex& v2, Vertex& v3,const glm::vec4& color) {
	// ignore degenerate triangle case
	if (v1.y == v2.y && v1.y == v3.y) {
		return;
	}
	// sort three vertexs by y, v1.y>v2.y>v3.y
	if (v1.y < v2.y) {
		std::swap(v1, v2);
	}
	if (v1.y < v3.y) {
		std::swap(v1, v3);
	}
	if (v2.y < v3.y) {
		std::swap(v2, v3);
	}

	// draw model

}

void Renderer::renderByPixel() {

}