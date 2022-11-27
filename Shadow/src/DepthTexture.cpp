#include "DepthTexture.h"

#include <stb_image.h>

DepthTexture::DepthTexture(int width, int height, float* data) :Texture(width, height) {
	if (data == nullptr) {
		m_data = new float[width * height];
	}
	else {
		m_data = data;
	}
};

DepthTexture::~DepthTexture() {
	delete[] m_data;
	m_data = nullptr;
}

DepthTexture::DepthTexture(const DepthTexture& depthTexture):Texture(depthTexture.m_width, depthTexture.m_height){
	m_data = depthTexture.m_data;
}

DepthTexture& DepthTexture::operator=(const DepthTexture& depthTexture)
{
	m_width = depthTexture.m_width;
	m_height = depthTexture.m_height;
	m_data = depthTexture.m_data;
	return *this;
}

glm::vec4 DepthTexture::sample(const glm::vec2& uv) const
{
	if (this == nullptr) {
		return glm::vec4(1.f);
	}
	int x = static_cast<int>(uv.x * m_width);
	int y = static_cast<int>(uv.y * m_height);
	float r = *(m_data + (x + y * m_width));
	return glm::vec4(r);
}
