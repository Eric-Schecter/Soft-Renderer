#pragma once

#include <cstdint>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <vector>

#include "Vertex.h"
#include "Camera.h"
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"
#include "Light.h"
#include "Bufferbit.h"

class Pipeline
{
public:
	void setFramebuffer(uint32_t* pixels, float* depth, int width,int height);
	void clear(BUFFER_BIT buffer_bit);
	void render(Mesh* mesh);
	void setViewport(int x, int y, int width, int height);
	inline void setViewport(const glm::mat4& viewport) { m_viewport = viewport; };
	inline void setShader(Shader* shader) { m_shader = shader; };
	inline float* getZBuffer() { return m_zbuffer; };

private:
	void setPixel(int x, int y, const glm::vec4& color);
	uint32_t touint32(const glm::vec4& color) const;
	void drawTriangle(const std::vector<glm::vec4>& gl_FragCoord);

	uint32_t* m_colorbuffer;
	float* m_zbuffer;
	int m_width;
	int m_height;
	glm::mat4 m_viewport = glm::mat4(1.f);
	Shader* m_shader = nullptr;
};

