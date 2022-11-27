#pragma once

#include "Image.h"

struct Material
{
    Image* diffuseMap;
    Image* normalMap;
    float specular;
    int shininess;
};

