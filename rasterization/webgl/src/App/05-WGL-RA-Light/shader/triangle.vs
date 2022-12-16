#version 300 es

layout(location=0)in vec3 aPosition;
layout(location=1)in vec3 aNormal;
layout(location=2)in vec2 aUv;
layout(location=3)in vec3 aTangent;
layout(location=4)in vec3 aBiTangent;

uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uNormalMatrix;

out vec3 vNormal;
out vec3 vPosition;
out vec2 vUv;
out mat3 vTBN;

void main(){
    vNormal=(uNormalMatrix*vec4(aNormal,1)).xyz;
    vUv=aUv;
    vPosition=(uModelMatrix*vec4(aPosition,1.)).xyz;
    
    vec3 T=normalize(uNormalMatrix*vec4(aTangent,1.f)).xyz;
    vec3 N=normalize(uNormalMatrix*vec4(aNormal,1.f)).xyz;
    vec3 B=normalize(uNormalMatrix*vec4(aBiTangent,1.f)).xyz;
    vTBN=mat3(T,B,N);
    
    gl_Position=uProjectionMatrix*uViewMatrix*uModelMatrix*vec4(aPosition,1.f);
}
