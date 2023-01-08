#version 460

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUv;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBiTangent;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 norm;
} ubo;

layout(location = 0) out vec3 vNormal;
layout(location = 1) out vec3 vPosition;
layout(location = 2) out vec2 vUv;
layout(location = 3) out mat3 vTBN;

void main() {
    vNormal = (ubo.norm * vec4(aNormal,1)).xyz;
    vUv = aUv;
    vPosition = (ubo.model * vec4(aPosition,1.)).xyz;

    vec3 T = normalize(ubo.norm * vec4(aTangent,1.f)).xyz;
    vec3 N = normalize(ubo.norm * vec4(aNormal,1.f)).xyz;
    vec3 B = normalize(ubo.norm * vec4(aBiTangent,1.f)).xyz;
    vTBN = mat3(T,B,N);

    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(aPosition,1.f);
}
