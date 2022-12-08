#include "Renderer.h"

#include <vcruntime_string.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <algorithm>
#include <limits>
#include <vector>

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

// SDL accept BGR format
uint32_t Renderer::touint32(const glm::vec4& color) const {
	uint32_t b = (uint32_t)(color.r * 255.0f);
	uint32_t g = (uint32_t)(color.g * 255.0f);
	uint32_t r = (uint32_t)(color.b * 255.0f);
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

void Renderer::drawTriangle(const std::vector<glm::vec4>& gl_FragCoord) {
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

			// perspectively-correct-linear-interpolation
			// https://paroj.github.io/gltut/Texturing/Tut14%20Interpolation%20Redux.html
			// linear interpolation in clip space
			glm::vec3 bc_clip(bc_screen.x / gl_FragCoord[0].w,bc_screen.y / gl_FragCoord[1].w,bc_screen.z / gl_FragCoord[2].w);
			bc_clip /= (bc_clip.x + bc_clip.y + bc_clip.z);

			// linear interpolate in clip space for z computation
			float z = glm::dot(glm::vec3(gl_FragCoord[0].z, gl_FragCoord[1].z, gl_FragCoord[2].z), bc_clip);

			// point should be in the same hand side if it is inside triangle
			if (bc_screen.x < 0.f || bc_screen.y < 0.f || bc_screen.z < 0.f || m_zbuffer[static_cast<int>(x + y * m_width)] < z) {
				continue;
			}

			glm::vec4 gl_fragment;
			bool isDiscard = shader->fragment(bc_clip, gl_fragment);
			if (!isDiscard) {
				m_zbuffer[static_cast<int>(x + y * m_width)] = z;
				setPixel(x, y, gl_fragment);
			}
		}
	}
}

void Renderer::render(const std::vector<Vertex>& vertices,const std::vector<uint32_t>& indices,const Camera* camera,
	const glm::mat4& modelMatrix, AmbientLight* ambientLight,DirectionalLight* directionalLight,PointLight* pointLight,SpotLight* spotLight,Material* material) {
	
	// set shader uniforms
	shader->u_projection = camera->projection;
	shader->u_view = camera->view;
	shader->u_cameraPos = camera->pos;
	shader->u_model = modelMatrix;
	shader->u_material = material;
	auto normalMatrix = glm::transpose(glm::inverse(modelMatrix));
	shader->u_normalMatrix = normalMatrix;
	shader->u_ambientLight = ambientLight;
	shader->u_directionalLight = directionalLight;
	shader->u_pointLight = pointLight;
	shader->u_spotLight = spotLight;

	for (int i = 0; i < indices.size(); i+=3) {
		// clip space, vertex shader output
		const int size = 3;
		std::vector<glm::vec4> vertexClipspace;
		for (size_t j = 0; j < size; ++j) {
			vertexClipspace.push_back(
				shader->vertex(
				vertices[indices[i + j]].position,
				vertices[indices[i + j]].uv,
				vertices[indices[i + j]].tangent,
				vertices[indices[i + j]].normal,
				vertices[indices[i + j]].biTangent,
				j
			));
		}

		// perspective division -> NDC coordinate
		std::vector<glm::vec4> vertexNDC;
		for (auto vertex : vertexClipspace) {
			vertexNDC.push_back(vertex / vertex.w);
		}

		// screen space, fragment shader input
		std::vector<glm::vec2> vertexScreen;
		for (auto vertex : vertexNDC) {
			vertexScreen.push_back(glm::vec2(camera->viewport * vertex));
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