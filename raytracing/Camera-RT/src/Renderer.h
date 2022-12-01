#pragma once

#include <cstdint>
#include <glm/vec4.hpp>
#include <vector>
#include <memory>

#include "Ray.h"
#include "Primitive.h"
#include "Camera.h"

class Renderer
{
public:
	void setFramebuffer(uint32_t* pixels,int width,int height);
	void clear();
	void render(const std::vector<std::shared_ptr<Primitive>>& scene, std::shared_ptr<Camera> camera);

private:
	void setPixel(int x, int y, const glm::vec4& color);
	uint32_t touint32(const glm::vec4& color) const;
	glm::vec3 getColor(const Ray& ray, const std::vector<std::shared_ptr<Primitive>>& scene,int depth);
	glm::vec3 getRandomUnitVec();
	float random(float min, float max);

	uint32_t*  m_framebuffer = nullptr;
	int m_width = 0;
	int m_height = 0;
	int m_max = 50;
	int m_sampleCount = 10;
};

