#pragma once

#include "Texture.h"

class DepthTexture : public Texture
{
public:
	DepthTexture(int width, int height, float* data);
	~DepthTexture() override;
	DepthTexture(const DepthTexture&);
	DepthTexture& operator=(const DepthTexture&);

	inline void setData(float* data) { m_data = data; };

	glm::vec4 sample(const glm::vec2& uv) const;
	float* m_data = nullptr;
};

