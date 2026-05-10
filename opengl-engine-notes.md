# Understanding Your OpenGL Engine
### Cameras, Lighting, and the Render Pipeline — A Reference Companion

This document walks back through everything we built, with the *why* behind each piece — the math, and the architectural reasoning. It's written against your actual code (`Camera.h`, `OrbitCamera.h`, `lit_material`, the gltf loader) so you can read it side by side with what's on disk.

---

## 1. The Transform Pipeline, Briefly

Before cameras or lighting make sense, it helps to have the big picture clear: every vertex your GPU draws goes through a chain of matrix multiplications to get from "a point in the model's own coordinate space" to "a pixel on your screen."

```
clip_space_position = Projection * View * Model * vec4(localPosition, 1.0)
```

Each matrix answers a different question:

- **Model matrix** — "where does this object sit in the world?" (its position, rotation, scale)
- **View matrix** — "where is the camera, and which way is it looking?" Effectively, this moves the *entire world* so the camera sits at the origin looking down a fixed axis — it's the inverse of "where the camera is."
- **Projection matrix** — "how does 3D space squash onto a 2D screen?" This is where field of view, aspect ratio, and the near/far clipping planes come in.

Matrix multiplication is applied right-to-left: a vertex is first moved by `Model`, then that result is moved by `View`, then by `Projection`. Order matters enormously here — multiplying in the wrong order doesn't error, it just silently produces nonsense transforms, which is why this pattern shows up unchanged in every shader we wrote:

```glsl
gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
```

Everything below is really just "how do we *build* the View matrix (cameras) and what do we do with the result before it reaches the screen (lighting, transparency, depth)."

---

## 2. Cameras

### 2.1 The View Matrix, Conceptually

`glm::lookAt(eye, target, up)` builds an orthonormal (mutually perpendicular, unit-length) basis of three vectors — a "forward," a "right," and an "up" — anchored at the eye position, then constructs a matrix that transforms world space into that basis. You never call this directly with raw numbers; instead, both camera types compute `eye`, `target` (or an equivalent forward vector), and `up` each frame, and hand them to `lookAt`.

### 2.2 The Free-Fly Camera — Euler Angles

This camera stores two angles: **yaw** (rotation around the vertical axis — "which way am I facing left/right") and **pitch** (rotation up/down). These are spherical coordinates in disguise. The forward direction is derived as:

```
Front.x = cos(yaw) * cos(pitch)
Front.y = sin(pitch)
Front.z = sin(yaw) * cos(pitch)
```

This is the standard parametrization of a point on a unit sphere. Intuitively: `cos(pitch)` shrinks the horizontal (x/z) component as you tilt up or down (at pitch = 90°, `cos(90°) = 0`, so you're looking straight up with zero horizontal component left), while `sin(pitch)` grows the vertical component the same way.

From `Front`, the other two basis vectors fall out of cross products:

```
Right = normalize(cross(Front, WorldUp))
Up    = normalize(cross(Right, Front))
```

`cross(Front, WorldUp)` gives a vector perpendicular to *both* — by construction, this always lies in the horizontal plane, which is why strafing left/right never tilts you even when you're looking up or down. `Up` is then just "perpendicular to both Front and Right," which guarantees the full basis is orthonormal regardless of pitch.

**Why pitch gets clamped to ±89°:** at exactly ±90°, `Front` becomes purely vertical, `Right` becomes a zero-length vector (cross product of two parallel vectors is zero), and `normalize()` on a zero vector is undefined. Clamping just short of that avoids the singularity — this is a mild, practical case of **gimbal lock**.

### 2.3 The Orbit Camera — Spherical Coordinates Around a Target

Where the free-fly camera's angles describe a *looking direction*, the orbit camera's angles describe *where the eye sits relative to a target point*:

```
offset.x = Distance * cos(pitch) * cos(yaw)
offset.y = Distance * sin(pitch)
offset.z = Distance * cos(pitch) * sin(yaw)
Position = Target + offset
```

This is the same spherical parametrization as before, just scaled by `Distance` and added to `Target` instead of being used directly as a direction. `Distance` *is* your zoom level — scrolling just shrinks or grows this radius.

**The sign bug we hit, as a worked example:** `offset` points *from the target to the eye*. The camera's actual viewing direction is the opposite — *from the eye to the target* — which is `-offset` (normalized). When we first wrote WASD movement, the code computed a "forward" vector using the same formula as `offset`'s horizontal component, which is backwards from where the camera is actually looking. The fix was to compute forward directly as:

```cpp
glm::vec3 forward = glm::normalize(Target - GetPosition());
```

This is a good general lesson: when a vector's *meaning* shifts (here, "direction to the eye" vs. "direction of view"), the sign has to shift with it, even if the underlying angles are identical.

**WASD on an orbit camera** moves `Target`, not `Position` directly — `Position` is always derived from `Target + offset`, so moving the target re-centers the whole orbit around a new point. `forward` here intentionally includes pitch (so looking up and pressing forward climbs), while `right` is deliberately kept horizontal:

```cpp
glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0,1,0)));
```

Crossing with world-up always produces a horizontal result, exactly as in the free-fly camera — even though `forward` itself can point steeply up or down.

---

## 3. Getting Geometry to the GPU

Your `mesh::vertex` struct is a fixed-size, tightly packed block:

```cpp
struct vertex {
    glm::vec3 position;  // bytes  0-11
    glm::vec3 normal;    // bytes 12-23
    glm::vec2 uv;        // bytes 24-31
};
```

The GPU doesn't know what a "vertex struct" is — it just reads raw bytes from a buffer at a fixed **stride** (the size of one vertex, 32 bytes here) and a fixed **offset** within each vertex (where a given field starts). `glVertexAttribPointer` is the call that tells it how to slice that raw memory:

```cpp
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));
//                    ^location ^components  ^type            ^stride        ^byte offset into each vertex
```

The `1` here (the attribute location) is the contract between your C++ and your GLSL — it has to match `layout(location = 1) in vec3 aNormal;` exactly, or the GPU will happily feed normal data into whatever attribute *is* declared at location 1 (or vice versa), producing garbled-looking output with no compile error, since this mismatch is invisible to the shader compiler.

---

## 4. Normals and the Normal Matrix

A normal is a vector perpendicular to a surface, and lighting depends entirely on its direction (see Section 5). When an object moves or rotates, its normals need to rotate the same way — that part's intuitive. The non-intuitive part is **scale**.

Picture a sphere squashed flat along one axis (non-uniform scale). The *surface* flattens, and a normal that should now point more "outward along the squashed direction" instead gets dragged in the wrong direction if you scale it the same way as the geometry — the normal and the surface stop being perpendicular. Scaling positions and scaling normals are not the same operation once scale is non-uniform.

The correct transform for normals is the **inverse transpose** of the model matrix's upper 3×3 (the part without translation):

```cpp
glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
```

This is a standard (if non-obvious) result from linear algebra: if positions transform by a matrix `M`, vectors that must *stay perpendicular* to surfaces built from those positions transform by `(M⁻¹)ᵀ`. For rotation-only or uniform-scale transforms, this happens to equal `M` itself, which is why the bug is invisible until you scale something unevenly — another case where everything *looks* fine until the exact condition that breaks it shows up.

---

## 5. Lighting: The Blinn-Phong Model

Your fragment shader computes three additive terms and sums them:

```glsl
result = (ambient + diffuse + specular) * texColor.rgb;
```

**Ambient** is the simplest: a flat fraction of the light color, applied everywhere regardless of angle. It exists purely so that surfaces facing away from the light aren't pure black — a crude stand-in for all the indirect bounce light a real scene would have.

**Diffuse** follows Lambert's cosine law: a surface reflects light most strongly when it faces the light directly, and progressively less as the angle increases, hitting zero at 90° (grazing) and staying zero beyond that (light from behind a surface shouldn't illuminate it):

```glsl
float diff = max(dot(norm, lightDir), 0.0);
```

The dot product of two unit vectors *is* the cosine of the angle between them — that's the entire physical model in one line. `max(..., 0.0)` clamps out the "behind the surface" case, where the dot product would go negative.

**Specular** is the bright highlight you see when looking roughly toward the reflection of the light source. The classic Phong model reflects the light vector and compares it to the view vector; **Blinn-Phong** (what you're using) takes a cheaper, visually similar shortcut using a "halfway vector":

```glsl
vec3 halfwayDir = normalize(lightDir + viewDir);
float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
```

The intuition: the halfway vector points exactly between "toward the light" and "toward the camera." When the surface normal aligns with that halfway direction, you're in the sweet spot where the light would bounce straight into your eye. `shininess` controls how tight that hotspot is — raising it sharpens the highlight into a small, intense point (like polished metal); lowering it spreads a duller, broader glow (like matte plastic).

A **directional light** (what you have) models something infinitely far away, like the sun — its direction is constant across the whole scene, with no position and no falloff with distance. That's why `uLightDirection` is just a vector, not a position.

---

## 6. The Material / Uniform System

Your `material` class implements a generic, name-keyed uniform system rather than hardcoding `glUniform` calls per material type:

```cpp
material->register_uniform("uLightColor");    // once, at construction: "this name exists"
material->push_uniform("uLightColor", color); // every frame: "here's its current value"
material->set_uniforms(transform);             // walks the map, fires the actual glUniform* calls
```

Two GPU-side facts make this pattern work. First, `glGetUniformLocation` (called in `fill_uniform_locations`) has to run *after* the shader program is linked — locations don't exist before that — but it only needs to run **once**, since locations don't change for the lifetime of a compiled program. Second, sampler uniforms (`uBaseColorSampler`) don't hold a texture *object* — they hold a small integer naming a **texture unit** (0, 1, 2…), and the actual texture is bound to that unit separately with `glActiveTexture`/`glBindTexture`. `push_uniform("uBaseColorSampler", 0)` and `texture->bind(0)` are two halves of the same handshake: "sampler reads from unit 0" + "this texture is *in* unit 0."

---

## 7. Case Study: The Draw-Order Bug

This is worth understanding deeply, because it's not a graphics-specific bug — it's a general lesson about programming against a stateful API.

OpenGL has exactly **one** active shader program and **one** bound texture per unit at any given moment — it's a single global state machine, not something that tracks per-object state for you. Your original code had two separate loops:

```cpp
// pass 1: set uniforms/textures for every primitive
for (primitive : primitives) primitive->material->set_uniforms(...);

// pass 2 (separate!): draw every primitive
for (primitive : primitives) draw(primitive);
```

Each iteration of pass 1 overwrites the previous one's program and texture binding — by the time the loop ends, only the **last** primitive's state survives. Pass 2 then draws *every* primitive using that single leftover state, since nothing re-set it between draws.

A model with one primitive can't expose this, because "last" and "only" mean the same thing. A multi-primitive model exposes it immediately. The fix is a general principle, not just a one-off patch: **bind state and draw in the same breath**, per object, never in two passes that assume state survives across other objects' setup:

```cpp
for (primitive : primitives) {
    primitive->material->set_uniforms(...); // bind THIS one's state...
    draw(primitive);                        // ...then immediately draw with it
}
```

---

## 8. Depth Testing and Backface Culling

The **depth buffer** stores, per pixel, the distance-from-camera of whatever's currently the closest thing drawn there. With `GL_DEPTH_TEST` enabled, a new fragment only gets written if it's closer than what's already there — this is what lets a foreground object correctly hide something behind it, regardless of draw order. It needs to be cleared every frame (`GL_DEPTH_BUFFER_BIT`) or stale distances from the previous frame linger.

**Backface culling** skips rendering triangles that face away from the camera — for a closed, solid mesh, you can never see its back-facing triangles anyway (the front ones are always in the way), so culling them is free performance. "Which way a triangle faces" is determined by **winding order** — the order its vertices appear in, projected onto the screen. By convention, counter-clockwise (`GL_CCW`) is treated as front-facing; this has to match how your geometry was actually authored/exported, or culling removes the wrong faces (the model can vanish, or look inside-out).

This combination (or lack of it) was the cause of your "hollow and transparent-looking" symptom: without proper depth/cull setup, you can end up seeing interior or back-facing geometry that should have been hidden, which reads visually as translucency even though nothing was actually blending.

**Near/far planes** (the last two arguments to `glm::perspective`) set the range of distances that get rendered at all — anything closer than `near` or farther than `far` is clipped away entirely, which is why far objects were disappearing until the far plane was raised. One subtlety: depth-buffer precision isn't distributed evenly across that range — it's concentrated close to the near plane and gets sparse near the far plane, so pushing `far` very high while keeping `near` very small increases the risk of **z-fighting** (flickering between overlapping surfaces) at a distance. Keeping the near/far ratio as small as your scene allows helps avoid this.

---

## 9. Transparency: Blending vs. Alpha Cutout

There are two genuinely different ways to handle "this pixel isn't fully opaque," and they suit different situations.

**Blending** mixes a new fragment's color with what's already in the framebuffer, weighted by alpha:

```cpp
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
```

This produces smooth, gradient transparency (glass, smoke, fading edges) — but it only looks correct if transparent surfaces are drawn back-to-front relative to the camera, since blending is order-dependent. That sorting requirement is real extra complexity.

**Alpha cutout** (what fixed your trees) skips that entirely with a binary decision per pixel:

```glsl
if (texColor.a < uAlphaCutoff) discard;
```

`discard` throws the fragment away before it's written anywhere — no color, no depth write, nothing. Since each pixel is either fully there or not there at all, there's no partial blending and therefore no sorting requirement; it works correctly with ordinary opaque depth testing. This suits foliage well because leaf-cutout textures are conceptually binary (leaf shape vs. not-leaf-shape), not a smooth gradient — and it explains why the un-discarded "transparent" areas were showing up as solid black: that color data was still being written to the framebuffer at full opacity, since nothing told the GPU to skip those pixels.

---

## 10. Where This Leaves You

Everything above is now sitting in working code you've tested against real, non-trivial models (a multi-primitive temple, foliage with cutout textures). The next layer — normal maps, tangent vectors, and eventually a fuller PBR (physically-based) material model — builds directly on the normal-matrix and lighting math in Sections 4–5, so revisiting those sections before going there should make the jump much smaller than it looks from the outside.
