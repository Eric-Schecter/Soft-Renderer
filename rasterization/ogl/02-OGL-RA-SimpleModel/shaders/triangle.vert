#version 460

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUv;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBiTangent;

out vec3 vNormal;

void main() {
    vNormal = aNormal;
    vec3 position = aPosition;
    position.z*=-1.f;

    gl_Position = vec4(position,1.f);
}
