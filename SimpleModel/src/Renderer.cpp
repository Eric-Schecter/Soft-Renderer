#include "Renderer.h"

#include <vcruntime_string.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <algorithm>
#include <limits>

void Renderer::setFramebuffer(uint32_t* pixels,	int width,int height) {
	m_framebuffer = pixels;
	m_zbuffer = new float[width * height];
	for (auto i = 0; i < width * height; ++i) {
		m_zbuffer[i] = -std::numeric_limits<float>::max();
;	}
	m_width = width;
	m_height = height;
}

void Renderer::clear() {
	memset(m_framebuffer,0,sizeof(uint32_t) * m_width * m_height);
}

uint32_t Renderer::touint32(const glm::vec4& color) const {
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

glm::vec3 barycentric(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, const glm::vec3& v4) {
	glm::vec3 a(v3.x-v1.x,v2.x-v1.x,v1.x-v4.x);
	glm::vec3 b(v3.y-v1.y,v2.y-v1.y,v1.y-v4.y);
	glm::vec3 c = glm::cross(a,b);

	// ignore degenerated triangle case
	if (std::abs(c.z)<=0.01f) {
		return glm::vec3(-1, 1, 1);
	}

	return glm::vec3(1.f-(c.x+c.y)/c.z,c.y/c.z,c.x/c.z);
}

void Renderer::drawTriangle( glm::vec3& v1,  glm::vec3& v2,  glm::vec3& v3, const glm::vec4& color) {
	glm::vec2 boundingBoxMin(m_width - 1, m_height - 1);
	glm::vec2 boundingBoxMax(0, 0);
	glm::vec3 v[] = { v1,v2,v3 };

	for (auto i = 0; i < 3; ++i) {
		boundingBoxMin.x = std::max(0.f, std::min(v[i].x, boundingBoxMin.x));
		boundingBoxMin.y = std::max(0.f, std::min(v[i].y, boundingBoxMin.y));

		boundingBoxMax.x = std::min(m_width - 1.f, std::max(v[i].x, boundingBoxMax.x));
		boundingBoxMax.y = std::min(m_height - 1.f, std::max(v[i].y, boundingBoxMax.y));
	}

	glm::vec3 v4;
	// should convert to int for pixel when loop
	for (int x = static_cast<int>(boundingBoxMin.x); x <= static_cast<int>(boundingBoxMax.x); ++x) {
		for (int y = static_cast<int>(boundingBoxMin.y); y <= static_cast<int>(boundingBoxMax.y); ++y) {
			v4.x = x;
			v4.y = y;
			glm::vec3 pos = barycentric(v1, v2, v3, v4);
			if (pos.x < 0.f || pos.y < 0.f || pos.z < 0.f) {
				continue;
			}
			v4.z = 0.f;
			for (auto i = 0; i < 3; ++i) {
				v4.z += v[i].z *pos[i];
			}
			if (m_zbuffer[static_cast<int>(x + y * m_width)] < v4.z) {
				m_zbuffer[static_cast<int>(x + y * m_width)] = v4.z;
				setPixel(x, y, color);
			}
		}
	}
}

void Renderer::render(const std::vector<Vertex>& vertices,const std::vector<uint32_t>& indices,const glm::vec4& color) {
	const glm::vec3 offset(1.f,-1.f, 1.f);
	const glm::vec3 scale(m_width / 2, -m_height / 2, 1.f);

	const glm::vec3 light = glm::normalize(glm::vec3(0,0,1));

	for (auto i = 0; i < indices.size(); i+=3) {
		// model2world
		glm::vec3 p1 = (vertices[indices[i]].position + offset) * scale;
		glm::vec3 p2 = (vertices[indices[static_cast<size_t>(i) + 1]].position + offset) * scale;
		glm::vec3 p3 = (vertices[indices[static_cast<size_t>(i) + 2]].position + offset) * scale;
		glm::vec3 ab = glm::normalize(vertices[indices[static_cast<size_t>(i) + 1]].position- vertices[indices[i]].position);
		glm::vec3 ac = glm::normalize(vertices[indices[static_cast<size_t>(i) + 2]].position- vertices[indices[i]].position);
		glm::vec3 n = glm::normalize(glm::cross(ab,ac));
		auto intensity = glm::dot(n,light);
		if (intensity > 0) {
			drawTriangle(p1, p2, p3, color * intensity);
		}
	}	
}