// FRAGMENT_TEXTURED
#version 330 core

in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uBaseColorSampler;

void main() {
    FragColor = texture(uBaseColorSampler, vUV);
}