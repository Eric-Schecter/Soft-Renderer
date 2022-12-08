#pragma once

#include "ColorTexture.h"

struct Material
{
    ColorTexture* diffuseMap;
    ColorTexture* normalMap;
    float specular;
    int shininess;
};

