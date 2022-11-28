#pragma once

#include <cstdint>
#include <glm/vec4.hpp>

#include "Ray.h"

class Renderer
{
public:
	void setFramebuffer(uint32_t* pixels,int width,int height);
	void clear();
	void render();

private:
	void setPixel(int x, int y, const glm::vec4& color);
	uint32_t touint32(const glm::vec4& color) const;
	bool hitSphere(const glm::vec3& center,float radius,const Ray& ray);
	glm::vec4 getColor(const Ray& ray);

	uint32_t*  m_framebuffer;
	int m_width;
	int m_height;
};

