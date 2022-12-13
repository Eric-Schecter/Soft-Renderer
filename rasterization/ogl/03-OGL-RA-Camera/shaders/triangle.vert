#version 460

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUv;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBiTangent;

// basic
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uModelMatrix;

//// ubo style
//layout(std140) uniform UniformBufferObject {
//    mat4 uViewMatrix;
//    mat4 uProjectionMatrix;
//    mat4 uModelMatrix;
//} ubo;

out vec3 vNormal;

// explicitly declare when use program pipeline
out gl_PerVertex
{
  vec4 gl_Position;
  float gl_PointSize;
  float gl_ClipDistance[];
};

void main() {
    vNormal = aNormal;
    vec3 position = aPosition;

    // ubo style
//    gl_Position = ubo.uProjectionMatrix * ubo.uViewMatrix * ubo.uModelMatrix * vec4(position,1.f);

    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(position,1.f);
}
