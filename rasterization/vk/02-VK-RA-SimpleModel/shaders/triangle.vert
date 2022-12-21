#version 460

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUv;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBiTangent;

layout(location = 0) out vec3 vNormal;

void main() {
    vNormal = aNormal;
    gl_Position = vec4(aPosition,1.f);
}
