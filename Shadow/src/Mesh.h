#pragma once

#include <vector>
#include <glm/mat4x4.hpp>

#include "Vertex.h"
#include "Material.h"

struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    Material* material;
    glm::mat4 modelMatrix;
};

