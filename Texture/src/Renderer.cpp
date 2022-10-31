#include "Renderer.h"

#include <vcruntime_string.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <algorithm>
#include <limits>

void Renderer::setFramebuffer(uint32_t* pixels,	int width,int height) {
	m_framebuffer = pixels;
	m_zbuffer = new float[width * height];
	for (auto i = 0; i < width * height; ++i) {
		m_zbuffer[i] = 1.f; // z is clamped between -1 and 1, 1 is far from the viewer, ndc coordinate is left-handed
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

// https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/rasterization-stage
// https://github.com/ssloy/tinyrenderer/wiki/Lesson-3:-Hidden-faces-removal-(z-buffer)
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

void Renderer::drawTriangle( glm::vec3& v1,  glm::vec3& v2,  glm::vec3& v3) {
	glm::vec2 boundingBoxMin(m_width - 1, m_height - 1);
	glm::vec2 boundingBoxMax(0, 0);
	glm::vec3 v[] = { v1,v2,v3 };

	for (auto i = 0; i < 3; ++i) {
		boundingBoxMin.x = std::max(0.f, std::min(v[i].x, boundingBoxMin.x));
		boundingBoxMin.y = std::max(0.f, std::min(v[i].y, boundingBoxMin.y));

		boundingBoxMax.x = std::min(m_width - 1.f, std::max(v[i].x, boundingBoxMax.x));
		boundingBoxMax.y = std::min(m_height - 1.f, std::max(v[i].y, boundingBoxMax.y));
	}

	// should convert to int for pixel when loop
	for (int x = static_cast<int>(boundingBoxMin.x); x <= static_cast<int>(boundingBoxMax.x); ++x) {
		for (int y = static_cast<int>(boundingBoxMin.y); y <= static_cast<int>(boundingBoxMax.y); ++y) {
			glm::vec3 v4(x, y, 0.f);
			glm::vec3 pos = barycentric(v1, v2, v3, v4); 
			if (pos.x < 0.f || pos.y < 0.f || pos.z < 0.f) { // point should be in the same hand side if it is inside triangle
				continue;
			}
			for (auto i = 0; i < 3; ++i) {
				v4.z += v[i].z *pos[i];
			}
			if (m_zbuffer[static_cast<int>(x + y * m_width)] > v4.z) {
				glm::vec3 color = glm::vec3(1.f,1.f,1.f);
				bool isDiscard = shader->fragment(pos,color);
				if (!isDiscard) {
					m_zbuffer[static_cast<int>(x + y * m_width)] = v4.z;
					setPixel(x, y, glm::vec4(color, 1.f));
				}
			}
		}
	}
}

void Renderer::render(const std::vector<Vertex>& vertices,const std::vector<uint32_t>& indices,const Camera* camera,
	const glm::mat4& modelMatrix, Image* diffuse, Image* normal) {
	glm::vec3 light = glm::normalize(glm::vec3(0,0,1.f));

	// set shader uniforms
	shader->u_projection = camera->projection;
	shader->u_view = camera->view;
	shader->u_model = modelMatrix;
	shader->u_light = light;
	shader->u_diffuse = diffuse;
	shader->u_normal = normal;

	for (auto i = 0; i < indices.size(); i+=3) {
		// clip space, vertex shader output
		const int size = 3;
		glm::vec4 p[size];
		for (int j = 0; j < size; ++j) {
			p[j]= shader->vertex(
				vertices[indices[static_cast<size_t>(i) + j]].position,
				vertices[indices[static_cast<size_t>(i) + j]].uv,
				j
			);
		}

		// perspective division -> NDC coordinate
		for (int j = 0; j < size; ++j) {
			p[j] /= p[j].w;
		}

		// screen space, fragment shader input
		glm::vec3 pss[size];
		for (int j = 0; j < size; ++j) {
			pss[j] = camera->viewport * p[j];
		}

		drawTriangle(pss[0], pss[1], pss[2]);
	}	
}