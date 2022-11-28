#include "Renderer.h"

#include <vcruntime_string.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <algorithm>

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

bool Renderer::hitSphere(const glm::vec3& center, float radius, const Ray& ray) {
	glm::vec3 oc = ray.origin - center;
	float a = glm::dot(ray.direction, ray.direction);
	float b = 2.f * glm::dot(oc, ray.direction);
	float c = glm::dot(oc, oc) - radius * radius;
	float discriminant = b * b - 4.f * a * c;
	return discriminant > 0.f;
}

glm::vec4 Renderer::getColor(const Ray& ray) {
	if (hitSphere(glm::vec3(0, 0, 0), 0.25, ray)) {
		return glm::vec4(1.f);
	}
	return glm::vec4(0.f,0.f,0.f,1.f);
}

void Renderer::render() {
	glm::vec3 origin(0.f,0.f,1.f);
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
			glm::vec4 color = getColor(ray);
			setPixel(w,h, color);
		}
	}
}