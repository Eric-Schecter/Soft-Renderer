#version 300 es

precision mediump float;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

in vec3 vNormal;
in vec3 vPosition;
in vec2 vUv;
in mat3 vTBN;

out vec4 fColor;

void main(){
    vec4 color = texture(diffuseMap,vUv);
    vec3 normal = texture(normalMap,vUv).xyz;
    normal = normalize(normal * 2. - vec3(1.));
    normal = normalize(vTBN * normal);

    vec4 ambientLight = vec4(.1f);
    vec3 lightDir = vec3(-1.f,-1.f,1.f);
    vec4 lightColor = vec4(1.f) * clamp(dot(normalize(lightDir),normal),0.f,1.f) + ambientLight;

    fColor = color * lightColor;  
}
