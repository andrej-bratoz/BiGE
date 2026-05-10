// FRAGMENT_TEXTURED_LIT
#version 330 core
 
in vec2 vUV;
in vec3 vFragPos;
in mat3 vTBN;
 
out vec4 FragColor;
 
uniform sampler2D uBaseColorSampler;
uniform sampler2D uNormalSampler;
 
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
 
    // Sample the normal map and unpack it from [0,1] (texture storage range)
    // to [-1,1] (actual direction range), then carry it from tangent space
    // into world space via the TBN matrix built in the vertex shader.
    vec3 tangentNormal = texture(uNormalSampler, vUV).rgb * 2.0 - 1.0;
    vec3 norm = normalize(vTBN * tangentNormal);
 
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
 