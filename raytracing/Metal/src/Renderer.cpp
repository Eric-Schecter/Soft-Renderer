#include "Renderer.h"

#include <vcruntime_string.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <algorithm>
#include <cstdlib>
#include <ctime>

Renderer::Renderer() {
	std::srand(time(NULL));
}

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

float Renderer::random(float min, float max) {
	return std::rand() * (max-min)/RAND_MAX + min;
}

glm::vec3 Renderer::getRandomUnitVec() {
	while (true) {
		glm::vec3 vec = glm::vec3(random(-1.f, 1.f), random(-1.f, 1.f), random(-1.f, 1.f));
		if (glm::length(vec) < 1.f) {
			return glm::normalize(vec);
		}
	}
}

glm::vec3 Renderer::getColor(const Ray& ray, const std::vector<std::shared_ptr<Primitive>>& scene,float depth) {
	if (depth <= 0) {
		return glm::vec3(0.f, 0.f, 0.f);
	}
	Record record;
	for (auto primitive : scene) {
		primitive->hit(ray, record);
	}
	if (record.isHit) {
		glm::vec3 target(record.pos+record.normal+getRandomUnitVec()); 
		Ray reflectedRay(record.pos, glm::normalize(target - record.pos));
		return 0.5f * getColor(reflectedRay,scene,depth-1);
	}
	auto t = 0.5f * (ray.direction.y + 1.0f);
	return (1.0f - t) * glm::vec3(1.0f, 1.0f, 1.0f) + t * glm::vec3(0.5f, 0.7f, 1.0f);
}

void Renderer::render(const std::vector<std::shared_ptr<Primitive>>& scene){
	glm::vec3 origin(0.f,0.25f,1.f);
	float ratio = static_cast<float>(m_width) / m_height;
	float uv_width = 2.f;
	float uv_height = uv_width / ratio;

	#pragma omp parallel for
	for (int h = 0; h < m_height; ++h) {
		for (int w = 0; w < m_width; ++w) {
			glm::vec2 uv(static_cast<float>(w)/m_width,static_cast<float>(h)/m_height);
			glm::vec3 target((uv.x-0.5f)*uv_width,(-uv.y+0.5f)*uv_height,0.f);
			glm::vec3 direction = target-origin;
			Ray ray(origin,direction);
			glm::vec3 color = getColor(ray, scene, m_max);
			setPixel(w,h,glm::vec4(color,1.f));
		}
	}
}