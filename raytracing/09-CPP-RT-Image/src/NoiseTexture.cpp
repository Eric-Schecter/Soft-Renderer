#include "NoiseTexture.h"

#include <glm/geometric.hpp>
#include <cmath>

#include "utils.h"

NoiseTexture::NoiseTexture(const glm::vec3& _color,float _scale) :color(_color),scale(_scale) {
	// v1,v2
	//for (int i = 0; i < NoiseTexture::count; ++i) {
	//	ranfloat[i] = utils::random(0.f, 1.f);
	//}
	for (int i = 0; i < NoiseTexture::count; ++i) {
		ranvec[i] = glm::normalize(glm::vec3(utils::random(-1.f, 1.f), utils::random(-1.f, 1.f), utils::random(-1.f, 1.f)));
	}
	perlinGeneratePerm(permX);
	perlinGeneratePerm(permY);
	perlinGeneratePerm(permZ);
}

void NoiseTexture::perlinGeneratePerm(int* perm) {
	int n = NoiseTexture::count;
	// init
	for (int i = 0; i < n; ++i) {
		perm[i] = i;
	}
	// random swap members
	for (int i = n - 1; i >= 0; --i) {
		int index = utils::random(0, i);
		int temp = perm[i];
		perm[i] = perm[index];
		perm[index] = temp;
;	}
}

// https://zhuanlan.zhihu.com/p/77496615
float NoiseTexture::trilinearInterp(float c[2][2][2], double u, double v, double w) const {
	float accum = 0.0f;
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 2; k++) {
				accum += (i * u + (1 - i) * (1 - u)) *
					(j * v + (1 - j) * (1 - v)) *
					(k * w + (1 - k) * (1 - w)) * c[i][j][k];
			}
		}
	}

	return accum;
}

float NoiseTexture::perlinInterp(glm::vec3 c[2][2][2], double u, double v, double w) const {
	// Hermite cubic
	auto uu = u * u * (3 - 2 * u);
	auto vv = v * v * (3 - 2 * v);
	auto ww = w * w * (3 - 2 * w);

	float accum = 0.0f;
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 2; k++) {
				glm::vec3 weight_v(u - i, v - j, w - k);
				accum += (i * uu + (1 - i) * (1 - uu)) *
					(j * vv + (1 - j) * (1 - vv)) *
					(k * ww + (1 - k) * (1 - ww)) *
					glm::dot(c[i][j][k], weight_v);
			}
		}
	}

	return accum;
}

float NoiseTexture::noise(const glm::vec3& p) const {
	// v1
	//auto i = static_cast<int>(p.x) & 255;
	//auto j = static_cast<int>(p.y) & 255;
	//auto k = static_cast<int>(p.z) & 255;
	//return ranfloat[permX[i] ^ permY[j] ^ permZ[k]];

	// v2
	//auto u = p.x - floor(p.x);
	//auto v = p.y - floor(p.y);
	//auto w = p.z - floor(p.z);

	//// Hermite cubic
	//u = u * u * (3 - 2 * u);
	//v = v * v * (3 - 2 * v);
	//w = w * w * (3 - 2 * w);

	//auto i = static_cast<int>(floor(p.x));
	//auto j = static_cast<int>(floor(p.y));
	//auto k = static_cast<int>(floor(p.z));

	//float c[2][2][2];
	//for (int di = 0; di < 2; ++di)
	//	for (int dj = 0; dj < 2; ++dj)
	//		for (int dk = 0; dk < 2; ++dk)
	//			c[di][dj][dk] = ranfloat[
	//				permX[(i + di) & 255] ^
	//				permY[(j + dj) & 255] ^
	//				permZ[(k + dk) & 255]
	//			];

	//return trilinearInterp(c, u, v, w);

	// v3 perlin noise
	auto u = p.x - floor(p.x);
	auto v = p.y - floor(p.y);
	auto w = p.z - floor(p.z);

	auto i = static_cast<int>(floor(p.x));
	auto j = static_cast<int>(floor(p.y));
	auto k = static_cast<int>(floor(p.z));

	glm::vec3 c[2][2][2];
	for (int di = 0; di < 2; ++di)
		for (int dj = 0; dj < 2; ++dj)
			for (int dk = 0; dk < 2; ++dk)
				c[di][dj][dk] = ranvec[
					permX[(i + di) & 255] ^
					permY[(j + dj) & 255] ^
					permZ[(k + dk) & 255]
				];

	return perlinInterp(c, u, v, w);
}

float NoiseTexture::turb(const glm::vec3& p, int depth) const {
	auto accum = 0.0;
	auto temp_p = p;
	auto weight = 1.0;

	for (int i = 0; i < depth; i++) {
		accum += weight * noise(temp_p);
		weight *= 0.5;
		temp_p *= 2;
	}

	return std::fabs(accum);
}

glm::vec3 NoiseTexture::sample(const glm::vec2& uv, const glm::vec3& p) const {
	//return color * (1.f + noise(scale * p));
	return color * 0.5f * (1 + std::sin(scale * p.z + scale * 4.f *turb(p)));
}