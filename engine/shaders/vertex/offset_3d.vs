#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform vec3 uOffset;
uniform float uScale;

void main() {
    gl_Position = vec4(aPos * uScale  + uOffset, 1.0);
}