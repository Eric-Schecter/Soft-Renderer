#include "Pipeline.h"

#include <vcruntime_string.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <algorithm>
#include <limits>
#include <vector>

void Pipeline::setFramebuffer(uint32_t* pixels, float* depth, int width,int height) {
	m_colorbuffer = pixels;
	m_zbuffer = depth;
	m_width = width;
	m_height = height;
}

void Pipeline::clear(BUFFER_BIT buffer_bit){
	switch (buffer_bit)
	{
	case BUFFER_BIT::COLOR_BUFFER_BIT:
		memset(m_colorbuffer, 0, sizeof(uint32_t) * m_width * m_height);
	case BUFFER_BIT::DEPTH_BUFFER_BIT:
		m_zbuffer = new float[m_width * m_height];
		for (int i = 0; i < m_width * m_height; ++i) {
			m_zbuffer[i] = 1.f;
		}
	case BUFFER_BIT::STENCIL_BUFFE_BIT:
		break;
	default:
		break;
	}
}

// SDL accept BGR format
uint32_t Pipeline::touint32(const glm::vec4& color) const {
	uint32_t b = (uint32_t)(color.r * 255.0f);
	uint32_t g = (uint32_t)(color.g * 255.0f);
	uint32_t r = (uint32_t)(color.b * 255.0f);
	uint32_t a = (uint32_t)(color.a * 255.0f);

	return (a << 24) | (r << 16) | (g << 8) | b;
}

void Pipeline::setPixel(int x,int y, const glm::vec4& color) {
	// ignore pixels outside bounding
	if (x >= m_width || y >= m_height || m_colorbuffer==nullptr) {
		return;
	}

	m_colorbuffer[y * m_width + x] = touint32(color);
}

void Pipeline::setViewport(int x, int y, int width, int height) {
	// set reverse y, coordinate start from left top corner
	m_viewport = glm::mat4(1.f);
	m_viewport = glm::translate(m_viewport, glm::vec3(x + width / 2.f, y + height / 2.f, 0));
	m_viewport = glm::scale(m_viewport, glm::vec3(width / 2.f, -height / 2.f, 10.f));

	//viewport = {
	//	width / 2.f, 0,            0,         0,
	//	0,           -height / 2.f, 0,         0,
	//	0,           0,            1.f,       0,
	//	x+width/2.f, y+height/2.f, 0,         1.f 
	//};
}

// https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/rasterization-stage
// https://github.com/ssloy/tinyPipeline/wiki/Lesson-3:-Hidden-faces-removal-(z-buffer)
glm::vec3 barycentric(const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3, const glm::vec2& v4) {
	glm::vec3 a(v3.x-v1.x,v2.x-v1.x,v1.x-v4.x);
	glm::vec3 b(v3.y-v1.y,v2.y-v1.y,v1.y-v4.y);
	glm::vec3 c = glm::cross(a,b);

	// ignore degenerated triangle case
	if (std::abs(c.z)<=0.01f) {
		return glm::vec3(-1, 1, 1);
	}

	return glm::vec3(1.f-(c.x+c.y)/c.z,c.y/c.z,c.x/c.z);
}

void Pipeline::drawTriangle(const std::vector<glm::vec4>& gl_FragCoord) {
	glm::vec2 boundingBoxMin(m_width - 1, m_height - 1);
	glm::vec2 boundingBoxMax(0, 0);

	for (auto i = 0; i < 3; ++i) {
		boundingBoxMin.x = std::max(0.f, std::min(gl_FragCoord[i].x, boundingBoxMin.x));
		boundingBoxMin.y = std::max(0.f, std::min(gl_FragCoord[i].y, boundingBoxMin.y));

		boundingBoxMax.x = std::min(m_width - 1.f, std::max(gl_FragCoord[i].x, boundingBoxMax.x));
		boundingBoxMax.y = std::min(m_height - 1.f, std::max(gl_FragCoord[i].y, boundingBoxMax.y));
	}

	// should convert to int for pixel when loop
	#pragma omp parallel for
	for (int x = static_cast<int>(boundingBoxMin.x); x <= static_cast<int>(boundingBoxMax.x); ++x) {
		for (int y = static_cast<int>(boundingBoxMin.y); y <= static_cast<int>(boundingBoxMax.y); ++y) {
			glm::vec3 bc_screen = barycentric(glm::vec2(gl_FragCoord[0]), glm::vec2(gl_FragCoord[1]), glm::vec2(gl_FragCoord[2]), glm::vec2(x,y));

			// point should be in the same hand side if it is inside triangle
			if (bc_screen.x < 0.f || bc_screen.y < 0.f || bc_screen.z < 0.f) {
				continue;
			}

			// perspectively-correct-linear-interpolation
			// https://paroj.github.io/gltut/Texturing/Tut14%20Interpolation%20Redux.html
			// linear interpolation in clip space
			glm::vec3 bc_clip(bc_screen.x / gl_FragCoord[0].w,bc_screen.y / gl_FragCoord[1].w,bc_screen.z / gl_FragCoord[2].w);
			// divide by area to barycentric coordinate
			bc_clip /= (bc_clip.x + bc_clip.y + bc_clip.z); 

			glm::vec4 gl_fragment(0.f);	
			bool isDiscard = m_shader->fragment(bc_clip, gl_fragment, gl_FragCoord);
			if (isDiscard) {
				continue;
			}

			// linear interpolate in clip space for z computation
			float z = glm::dot(glm::vec3(gl_FragCoord[0].z, gl_FragCoord[1].z, gl_FragCoord[2].z), bc_clip);
			bool depthTest = m_zbuffer[static_cast<int>(x + y * m_width)] > z;
			if (!depthTest) {
				continue;
			}
			
			m_zbuffer[static_cast<int>(x + y * m_width)] = z;
			setPixel(x, y, gl_fragment);
		}
	}
}

void Pipeline::render(Mesh* mesh) {
	if (m_shader == nullptr || mesh==nullptr) {
		return;
	}

	auto indices = mesh->indices;
	for (int i = 0; i < indices.size(); i+=3) {
		// clip space, vertex shader output
		const int size = 3;
		std::vector<glm::vec4> vertexClipspace;
		for (int j = 0; j < size; ++j) {
			vertexClipspace.push_back(m_shader->vertex(indices[i + j],j));
		}

		// perspective division -> NDC coordinate
		std::vector<glm::vec4> vertexNDC;
		for (auto vertex : vertexClipspace) {
			vertexNDC.push_back(vertex / vertex.w);
		}

		// screen space, fragment shader input
		std::vector<glm::vec2> vertexScreen;
		for (auto vertex : vertexNDC) {
			vertexScreen.push_back(glm::vec2(m_viewport * vertex));
		}

		std::vector<glm::vec4> gl_FragCoord(size);
		for (size_t j = 0; j < size; ++j) {
			gl_FragCoord[j].x = vertexScreen[j].x;
			gl_FragCoord[j].y = vertexScreen[j].y;
			gl_FragCoord[j].z = vertexNDC[j].z;
			gl_FragCoord[j].w = 1.f/vertexClipspace[j].w;
		}

		drawTriangle(gl_FragCoord);
	}	
}