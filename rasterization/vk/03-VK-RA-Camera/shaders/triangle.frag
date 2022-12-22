#version 460

layout(location = 0) in vec3 vNormal;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(vNormal,1.0);
}