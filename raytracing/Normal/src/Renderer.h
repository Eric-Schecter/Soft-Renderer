#pragma once

#include <cstdint>
#include <glm/vec4.hpp>
#include <vector>
#include <memory>

#include "Ray.h"
#include "Primitive.h"

class Renderer
{
public:
	void setFramebuffer(uint32_t* pixels,int width,int height);
	void clear();
	void render(const std::vector<std::shared_ptr<Primitive>>& scene);

private:
	void setPixel(int x, int y, const glm::vec4& color);
	uint32_t touint32(const glm::vec4& color) const;
	glm::vec4 getColor(const Ray& ray, const std::vector<std::shared_ptr<Primitive>>& scene);

	uint32_t*  m_framebuffer;
	int m_width;
	int m_height;
};

