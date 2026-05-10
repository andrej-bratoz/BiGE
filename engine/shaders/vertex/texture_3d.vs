// VERTEX_TEXTURED_3D
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform vec3 uOffset;
uniform float uScale;

out vec2 vUV;  // pass to fragment

void main() {
    gl_Position = vec4(aPosition * uScale + uOffset, 1.0);
    vUV = aUV;
}