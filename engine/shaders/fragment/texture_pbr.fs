// FRAGMENT_TEXTURED_PBR
#version 330 core

#define PI 3.14159265359

in vec2 vUV;
in vec3 vFragPos;
in mat3 vTBN;

out vec4 FragColor;

uniform sampler2D uBaseColorSampler;
uniform sampler2D uNormalSampler;
uniform sampler2D uMetallicRoughnessSampler; // glTF convention: G = roughness, B = metalness

uniform vec3 uLightDirection; // direction the light TRAVELS
uniform vec3 uLightColor;
uniform vec3 uAmbientColor;
uniform float uAmbientStrength;
uniform vec3 uViewPos;

uniform float uMetallicFactor;
uniform float uRoughnessFactor;
uniform float uAlphaCutoff;

// GGX / Trowbridge-Reitz normal distribution -- how aligned the surface's
// microfacets are with the halfway vector. Low roughness -> tight, bright peak.
float distributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float denom = NdotH2 * (a2 - 1.0) + 1.0;
    return a2 / max(PI * denom * denom, 0.0000001);
}

// Schlick-GGX geometry term -- microfacets shadowing/masking each other,
// applied once for the view direction and once for the light direction.
float geometrySchlickGGX(float NdotX, float roughness) {
    float r = roughness + 1.0;
    float k = (r * r) / 8.0; // direct-lighting remapping of roughness
    return NdotX / (NdotX * (1.0 - k) + k);
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    return geometrySchlickGGX(NdotV, roughness) * geometrySchlickGGX(NdotL, roughness);
}

// Fresnel-Schlick -- reflectance rises toward 1.0 at grazing angles.
// F0 is the reflectance straight-on: ~0.04 for non-metals, the surface's
// own albedo for metals (this is what makes gold look gold, not just shiny).
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main() {
    vec4 baseColorSample = texture(uBaseColorSampler, vUV);
    if (baseColorSample.a < uAlphaCutoff) {
        discard;
    }
    vec3 albedo = baseColorSample.rgb;

    vec3 mrSample = texture(uMetallicRoughnessSampler, vUV).rgb;
    float roughness = clamp(mrSample.g * uRoughnessFactor, 0.045, 1.0); // floor avoids dividing by ~0 at roughness 0
    float metallic  = clamp(mrSample.b * uMetallicFactor, 0.0, 1.0);

    vec3 tangentNormal = texture(uNormalSampler, vUV).rgb * 2.0 - 1.0;
    vec3 N = normalize(vTBN * tangentNormal);
    vec3 V = normalize(uViewPos - vFragPos);
    vec3 L = normalize(-uLightDirection);
    vec3 H = normalize(V + L);

    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    float NDF = distributionGGX(N, H, roughness);
    float G   = geometrySmith(N, V, L, roughness);
    vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 specular = (NDF * G * F) / max(4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0), 0.0001);

    // Energy conservation: kD + kS can't exceed 1, and metals have no diffuse term at all.
    vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);

    float NdotL = max(dot(N, L), 0.0);
    vec3 directLighting = (kD * albedo / PI + specular) * uLightColor * NdotL;

    // Ambient stays the flat approximation for now -- swapping this for real
    // image-based lighting is a separate, later step.
    vec3 ambient = uAmbientColor * uAmbientStrength * albedo;

    vec3 color = ambient + directLighting;

    FragColor = vec4(color, baseColorSample.a);
}
