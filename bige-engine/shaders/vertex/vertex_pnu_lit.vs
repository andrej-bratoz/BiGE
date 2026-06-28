// VERTEX_LIT_3D
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;

out vec2 vUV;
out vec3 vNormal;
out vec3 vFragPos;

void main() {
    vFragPos = vec3(uModel * vec4(aPosition, 1.0));
    vNormal  = normalize(uNormalMatrix * aNormal);
    vUV      = aUV;

    gl_Position = uProjection * uView * vec4(vFragPos, 1.0);
}
