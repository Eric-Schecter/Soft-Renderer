#pragma once

#include <gl/glew.h>

struct Material {
    GLint diffuseMap;
    GLint normalMap;
    float specular;
    int shininess;
};