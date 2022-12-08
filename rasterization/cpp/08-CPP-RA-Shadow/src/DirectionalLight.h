#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <memory>

#include "DepthTexture.h"

struct DirectionalLight
{
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;
    glm::mat4 shadowMatrix;
    std::shared_ptr<DepthTexture> shadowmap;
};