// VERTEX_LIT_3D
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(location = 3) in vec4 aTangent;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;

out vec2 vUV;
out vec3 vFragPos;
out mat3 vTBN;

void main() {
    vFragPos = vec3(uModel * vec4(aPosition, 1.0));
    vUV      = aUV;

    vec3 N = normalize(uNormalMatrix * aNormal);

    // Tangents lie IN the surface, so unlike normals they transform correctly
    // with the model matrix directly -- no inverse-transpose needed here.
    vec3 T = normalize(mat3(uModel) * aTangent.xyz);

    // Re-orthogonalize T against N (Gram-Schmidt). Independently transforming
    // T and N can leave them slightly non-perpendicular under non-uniform
    // scale; this keeps the TBN basis valid.
    T = normalize(T - dot(T, N) * N);

    // aTangent.w stores handedness (+1 or -1) -- flips the bitangent for
    // mirrored UV islands, which is common on most authored models.
    vec3 B = cross(N, T) * aTangent.w;

    vTBN = mat3(T, B, N);

    gl_Position = uProjection * uView * vec4(vFragPos, 1.0);
}
