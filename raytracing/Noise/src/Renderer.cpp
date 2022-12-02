#include "Renderer.h"

#include <vcruntime_string.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <algorithm>

#include "utils.h"

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

glm::vec3 Renderer::getColor(const Ray& ray, const std::vector<std::shared_ptr<Primitive>>& scene,int depth) {
	if (depth <= 0) {
		return glm::vec3(0.f, 0.f, 0.f);
	}
	Record record;
	for (auto primitive : scene) {
		primitive->hit(ray, record);
	}
	if (record.isHit) {
		Ray scatteredRay;
		glm::vec3 attenuation;
		if (record.material->scatter(ray, record, scatteredRay, attenuation)) {
			return attenuation * getColor(scatteredRay, scene, depth - 1);
		}
		return glm::vec3(0.f);
	}
	auto t = 0.5f * (ray.direction.y + 1.0f);
	return (1.0f - t) * glm::vec3(1.0f, 1.0f, 1.0f) + t * glm::vec3(0.5f, 0.7f, 1.0f);
}

void Renderer::gammaCorrect(glm::vec3& color,float gammaIndex) {
	color.x = std::pow(color.x, 1.f/gammaIndex);
	color.y = std::pow(color.y, 1.f/gammaIndex);
	color.z = std::pow(color.z, 1.f/gammaIndex);
}

void Renderer::render(const std::vector<std::shared_ptr<Primitive>>& scene, std::shared_ptr<Camera> camera){
	#pragma omp parallel for
	for (int h = 0; h < m_height; ++h) {
		for (int w = 0; w < m_width; ++w) {
			glm::vec3 color(0.f);
			for (int i = 0; i < m_sampleCount;++i) {
				float u = w + 0.5f + utils::random(-0.5f,0.5f);
				float v = h + 0.5f + utils::random(-0.5f,0.5f);
				Ray ray = camera->getRay(glm::vec2(u,v), glm::vec2(m_width, m_height));
				color += getColor(ray, scene, m_max);
			}
			color /= static_cast<float>(m_sampleCount);
			gammaCorrect(color,2.2f);
			setPixel(w,h,glm::vec4(color,1.f));
		}
	}
}