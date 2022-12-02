#pragma once

#include "Texture.h"

// refer to https://raytracing.github.io/books/RayTracingTheNextWeek.html
class NoiseTexture : public Texture
{
public:
	NoiseTexture(const glm::vec3& _color = glm::vec3(1.f),float _scale = 40.f);

	glm::vec3 sample(const glm::vec2& uv, const glm::vec3& p) const override;

private:
	float noise(const glm::vec3& p) const;
	float turb(const glm::vec3& p, int depth = 7) const;
	void perlinGeneratePerm(int* perm);
	float trilinearInterp(float c[2][2][2], double u, double v, double w) const;
	float perlinInterp(glm::vec3 c[2][2][2], double u, double v, double w) const;

	static const int count = 256;
	//float ranfloat[count];
	glm::vec3 ranvec[count];
	int permX[count];
	int permY[count];
	int permZ[count];
	glm::vec3 color;
	float scale;
};

