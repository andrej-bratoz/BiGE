// FRAGMENT_TEXTURED_LIT
#version 330 core
 
in vec2 vUV;
in vec3 vNormal;
in vec3 vFragPos;
 
out vec4 FragColor;
 
uniform sampler2D uBaseColorSampler;
 
uniform vec3 uLightDirection; // direction the light TRAVELS, e.g. normalize(vec3(-0.3, -1.0, -0.2))
uniform vec3 uLightColor;
uniform vec3 uViewPos;
 
uniform float uAmbientStrength;
uniform float uSpecularStrength;
uniform float uShininess;
uniform float uAlphaCutoff; // fragments with alpha below this are discarded entirely (foliage cutouts, fences, etc.)
 
void main() {
    vec4 texColor = texture(uBaseColorSampler, vUV);
    if (texColor.a < uAlphaCutoff) {
        discard;
    }
 
    vec3 norm    = normalize(vNormal);
    vec3 lightDir = normalize(-uLightDirection); // points FROM the surface TOWARD the light
 
    vec3 ambient = uAmbientStrength * uLightColor;
 
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;
 
    vec3 viewDir    = normalize(uViewPos - vFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec      = pow(max(dot(norm, halfwayDir), 0.0), uShininess);
    vec3 specular   = uSpecularStrength * spec * uLightColor;
 
    vec3 result   = (ambient + diffuse + specular) * texColor.rgb;
 
    FragColor = vec4(result, texColor.a);
}
 