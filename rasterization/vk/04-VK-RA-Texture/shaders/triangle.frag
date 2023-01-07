#version 460

layout(binding = 1) uniform sampler2D diffuseMap;
layout(binding = 2) uniform sampler2D normalMap;

layout(location = 0) in vec3 vNormal;
layout(location = 1) in vec3 vPosition;
layout(location = 2) in vec2 vUv;
layout(location = 3) in mat3 vTBN;

layout(location = 0) out vec4 outColor;
    
void main() {
    vec4 color = texture(diffuseMap,vUv);
    vec3 normal = texture(normalMap,vUv).xyz;
    normal = normalize(normal * 2.f - vec3(1.f));
    normal = normalize(vTBN * normal);

    vec4 ambientLight = vec4(.1f);
    vec3 lightDir = vec3(-1.f,-1.f,1.f);
    vec4 lightColor = vec4(1.f) * clamp(dot(normalize(lightDir),normal),0.f,1.f) + ambientLight;

    outColor = color * lightColor;
}