#pragma once

#include <cstdint>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <vector>

#include "Vertex.h"
#include "Camera.h"
#include "Shader.h"
#include "Image.h"

class Renderer
{
public:
	void setFramebuffer(uint32_t* pixels,int width,int height);
	void clear();
	void render(
		const std::vector<Vertex>& vertices,
		const std::vector<uint32_t>& indices, 
		const Camera* camera,
		const glm::mat4& modelMatrix,
		Image* diffuse,
		Image* normal
		);

private:
	void setPixel(int x, int y, const glm::vec4& color);
	uint32_t touint32(const glm::vec4& color) const;
	void drawTriangle( glm::vec3& v1,  glm::vec3& v2,  glm::vec3& v3);
	Shader* shader = new Shader(); // use one shader for renderer for now

	uint32_t*  m_framebuffer;
	float* m_zbuffer;
	int m_width;
	int m_height;
};

