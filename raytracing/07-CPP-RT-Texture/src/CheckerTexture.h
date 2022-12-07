#pragma once

#include "Texture.h"

#include <memory>

class CheckerTexture: public Texture
{
public:
	CheckerTexture(std::shared_ptr<Texture> _odd, std::shared_ptr<Texture> _even) :odd(_odd),even(_even){}

	glm::vec3 sample(const glm::vec2& uv, const glm::vec3& p) const override;

private:
	std::shared_ptr<Texture> odd;
	std::shared_ptr<Texture> even;
};

