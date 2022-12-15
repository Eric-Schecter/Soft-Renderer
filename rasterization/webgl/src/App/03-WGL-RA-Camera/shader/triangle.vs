#version 300 es

layout(location=0)in vec3 aPosition;
layout(location=1)in vec3 aNormal;
layout(location=2)in vec2 aUv;
layout(location=3)in vec3 aTangent;
layout(location=4)in vec3 aBiTangent;

uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uModelMatrix;

out vec3 vNormal;

void main(){
    vNormal=aNormal;
    gl_Position=uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPosition,1.f);
}
