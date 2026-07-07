#include <bige-gltf-parser.h>
#include <bige-logging.h>
#include <bige-material.h>
#include <bige-settings.h>
#include <materials/bige-materials.h>
//
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/mat4x2.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#define TINYGLTF_USE_RAPIDJSON
#include <tiny_gltf.h>

namespace bige {

    // implement gltf material chooser
    bige::material* materials::choose_material_tglf(void* platform_material,
        std::shared_ptr<bige::texture> tex,
        std::shared_ptr<bige::texture> normal_tex,
        std::shared_ptr<bige::texture> rough_met) {
        
        tinygltf::Material* mat = static_cast<tinygltf::Material*>(platform_material);
        if (!mat || (!tex && !normal_tex)) {
            return new  bige::materials::solid_color_material(glm::vec4(1.0f));
		}

        const bool isUnlit = mat->extensions.count("KHR_materials_unlit") > 0;
        const bool hasNormal = mat->normalTexture.index != -1;
        const auto& pbr = mat->pbrMetallicRoughness;
        const bool hasAlbedoTex = pbr.baseColorTexture.index != -1;
        const bool hasMetRoughTex = pbr.metallicRoughnessTexture.index != -1;
        const bool hasPBRFactors = pbr.metallicFactor != 1.0 || pbr.roughnessFactor != 1.0;

        if (isUnlit) {
            if (hasAlbedoTex) {
                auto val = new bige::materials::textured_material();
                val->base_color_texture = tex;
                val->normal_texture = normal_tex;
                val->metallic_roughness_texture = rough_met;
                return val;
            } else {
                const auto& c = mat->pbrMetallicRoughness.baseColorFactor;
                return new bige::materials::solid_color_material(glm::vec4(c[0], c[1], c[2], c[3]));
            }
        }

        if (!hasAlbedoTex && !hasMetRoughTex && !hasPBRFactors && !hasNormal) {
            auto val = new bige::materials::lit_material();
            val->base_color_texture = tex;
            val->normal_texture = normal_tex;
            val->metallic_roughness_texture = rough_met;

            float roughness = static_cast<float>(mat->pbrMetallicRoughness.roughnessFactor);
            float metallic = static_cast<float>(mat->pbrMetallicRoughness.metallicFactor);

            // roughness 0 = mirror = very shiny, roughness 1 = dull
            float shininess = (1.0f - roughness) * 128.0f;  // range [0, 128]
            float specular_strength = glm::mix(0.04f, 1.0f, metallic); // non-metals ~4%, metals ~100%
            float ambient_strength = 0.1f; // glTF has no ambient concept, just pick a constant

			val->shininess = shininess;
			val->specular_strength = specular_strength;

            if (mat->alphaMode == "MASK")
                val->alpha_cutoff = static_cast<float>(mat->alphaCutoff);

            return val;
        }

        const bool hasPBR = hasMetRoughTex || hasPBRFactors;

        if (hasAlbedoTex && hasPBR && hasNormal) {
            auto val = new bige::materials::pbr_material();
            val->base_color_texture = tex;
            val->normal_texture = normal_tex;
            val->metallic_roughness_texture = rough_met;

            float metallic_factor = static_cast<float>(mat->pbrMetallicRoughness.metallicFactor);
            float roughness_factor = static_cast<float>(mat->pbrMetallicRoughness.roughnessFactor);

            float alpha_cutoff = 0.5f;
            if (mat->alphaMode == "MASK")
                alpha_cutoff = static_cast<float>(mat->alphaCutoff);

			val->metallic_factor = metallic_factor;
			val->roughness_factor = roughness_factor;
			val->alpha_cutoff = alpha_cutoff;

            return val;
        }
        
        if (hasAlbedoTex && hasPBR) {
            auto val = new bige::materials::pbr_material();
            val->base_color_texture = tex;
            val->normal_texture = normal_tex;
            val->metallic_roughness_texture = rough_met;

            float metallic_factor = static_cast<float>(mat->pbrMetallicRoughness.metallicFactor);
            float roughness_factor = static_cast<float>(mat->pbrMetallicRoughness.roughnessFactor);

            float alpha_cutoff = 0.5f;
            if (mat->alphaMode == "MASK")
                alpha_cutoff = static_cast<float>(mat->alphaCutoff);

            val->metallic_factor = metallic_factor;
            val->roughness_factor = roughness_factor;
            val->alpha_cutoff = alpha_cutoff;

            return val;
        }

        if (hasAlbedoTex && hasNormal) {
            auto val = new bige::materials::lit_material_normals();
            val->base_color_texture = tex;
            val->normal_texture = normal_tex;
            val->metallic_roughness_texture = rough_met;

            float roughness = static_cast<float>(mat->pbrMetallicRoughness.roughnessFactor);
            float metallic = static_cast<float>(mat->pbrMetallicRoughness.metallicFactor);

            // roughness 0 = mirror = very shiny, roughness 1 = dull
            float shininess = (1.0f - roughness) * 128.0f;  // range [0, 128]
            float specular_strength = glm::mix(0.04f, 1.0f, metallic); // non-metals ~4%, metals ~100%
            float ambient_strength = 0.1f; // glTF has no ambient concept, just pick a constant

            val->shininess = shininess;
            val->specular_strength = specular_strength;

            if (mat->alphaMode == "MASK")
                val->alpha_cutoff = static_cast<float>(mat->alphaCutoff);

            return val;
        }
        if (hasAlbedoTex) {
            auto val = new bige::materials::lit_material();
            val->base_color_texture = tex;
            val->normal_texture = normal_tex;
            val->metallic_roughness_texture = rough_met;

            float roughness = static_cast<float>(mat->pbrMetallicRoughness.roughnessFactor);
            float metallic = static_cast<float>(mat->pbrMetallicRoughness.metallicFactor);

            // roughness 0 = mirror = very shiny, roughness 1 = dull
            float shininess = (1.0f - roughness) * 128.0f;  // range [0, 128]
            float specular_strength = glm::mix(0.04f, 1.0f, metallic); // non-metals ~4%, metals ~100%
            float ambient_strength = 0.1f; // glTF has no ambient concept, just pick a constant

            val->shininess = shininess;
            val->specular_strength = specular_strength;

            if (mat->alphaMode == "MASK")
                val->alpha_cutoff = static_cast<float>(mat->alphaCutoff);

            return val;
        }
        return new bige::materials::solid_color_material(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));
    }


    static const unsigned char* accessor_data(const tinygltf::Model& gltf, int accessor_index, size_t& out_count, 
                                              size_t& out_stride_bytes, int& out_component_type, int& out_type) {
        const auto& accessor = gltf.accessors[accessor_index];
        const auto& buffer_view = gltf.bufferViews[accessor.bufferView];
        const auto& buffer = gltf.buffers[buffer_view.buffer];

        out_count = accessor.count;
        out_component_type = accessor.componentType;  // GL_FLOAT, GL_UNSIGNED_SHORT, ...
        out_type = accessor.type;                     // TINYGLTF_TYPE_VEC3, ...

        // stride: explicit in bufferView, or implicit (tight packing of element size)
        size_t element_size = tinygltf::GetComponentSizeInBytes(accessor.componentType) * tinygltf::GetNumComponentsInType(accessor.type);
        out_stride_bytes = buffer_view.byteStride > 0 ? buffer_view.byteStride : element_size;

        return buffer.data.data() + buffer_view.byteOffset + accessor.byteOffset;
    }

    // read a vec3 accessor into std::vector<glm::vec3>
    static std::vector<glm::vec3> read_vec3(const tinygltf::Model& gltf, int accessor_idx) {
        size_t count, stride;
        int comp_type, type;
        auto* data = accessor_data(gltf, accessor_idx, count, stride, comp_type, type);

        std::vector<glm::vec3> result(count);
        for (size_t i = 0; i < count; i++) {
            const float* f = reinterpret_cast<const float*>(data + i * stride);
            result[i] = { f[0], f[1], f[2] };
        }
        return result;
    }

    static std::vector<glm::vec2> read_vec2(const tinygltf::Model& gltf, int accessor_idx) {
        size_t count, stride;
        int comp_type, type;
        auto* data = accessor_data(gltf, accessor_idx, count, stride, comp_type, type);

        std::vector<glm::vec2> result(count);
        for (size_t i = 0; i < count; i++) {
            const float* f = reinterpret_cast<const float*>(data + i * stride);
            result[i] = { f[0], f[1] };
        }
        return result;
    }

    static std::vector<glm::vec4> read_vec4(const tinygltf::Model& gltf, int accessor_idx) {
        size_t count, stride;
        int comp_type, type;
        auto* data = accessor_data(gltf, accessor_idx, count, stride, comp_type, type);

        std::vector<glm::vec4> result(count);
        for (size_t i = 0; i < count; i++) {
            const float* f = reinterpret_cast<const float*>(data + i * stride);
            result[i] = { f[0], f[1], f[2], f[3] };
        }
        return result;
    }

    // indices can be u8 / u16 / u32 — normalize to u32
    static std::vector<uint32_t> read_indices(const tinygltf::Model& gltf, int accessor_idx) {
        size_t count, stride;
        int comp_type, type;
        auto* data = accessor_data(gltf, accessor_idx, count, stride, comp_type, type);

        std::vector<uint32_t> result(count);
        for (size_t i = 0; i < count; i++) {
            const unsigned char* p = data + i * stride;
            switch (comp_type) {
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                result[i] = *p; break;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                result[i] = *reinterpret_cast<const uint16_t*>(p); break;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                result[i] = *reinterpret_cast<const uint32_t*>(p); break;
            default:
                result[i] = 0;  // unknown — log this in real code
            }
        }
        return result;
    }

    // ---------- mesh construction ----------
    std::unique_ptr<mesh> gltf_loader::build_mesh(const tinygltf::Model& gltf, const tinygltf::Primitive& prim) {
        auto pos_it = prim.attributes.find("POSITION");
        if (pos_it == prim.attributes.end()) {
            return nullptr;  // no geometry
        }
        auto positions = read_vec3(gltf, pos_it->second);

        // normals & uvs optional — fill with defaults if absent
        std::vector<glm::vec3> normals;
        auto norm_it = prim.attributes.find("NORMAL");
        if (norm_it != prim.attributes.end()) {
            normals = read_vec3(gltf, norm_it->second);
        } else {
            normals.assign(positions.size(), glm::vec3(0.0f, 1.0f, 0.0f));
        }

        std::vector<glm::vec2> uvs;
        auto uv_it = prim.attributes.find("TEXCOORD_0");
        if (uv_it != prim.attributes.end()) {
            uvs = read_vec2(gltf, uv_it->second);
        } else {
            uvs.assign(positions.size(), glm::vec2(0.0f, 0.0f));
        }

        std::vector<glm::vec4> tangents;
        auto tangent_it = prim.attributes.find("TANGENT");
        if (tangent_it != prim.attributes.end()) {
            tangents = read_vec4(gltf, tangent_it->second);
        }
        else {
            tangents.assign(positions.size(), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        }

        auto m = std::unique_ptr<mesh>(new mesh());  // private ctor; this matches your create_triangle_mesh pattern

        for (size_t i = 0; i < positions.size(); i++) {
            m->add_vertex(mesh::vertex(positions[i], normals[i], uvs[i], tangents[i]));
        }

        if (prim.indices >= 0) {
            auto idx = read_indices(gltf, prim.indices);
            for (uint32_t i : idx) m->add_index(i);
        }

        m->caulcuate_bounds();
        return m;
    }

    std::shared_ptr<bige::texture> resolve_base_color_texture(const tinygltf::Model& gltf, const tinygltf::Material& mat, const std::vector<std::shared_ptr<bige::texture>>& loaded_textures) {
        _log("resolving base color texture");
        // step 1: which TEXTURE does the material's baseColorTexture point to?
        int tex_idx = mat.pbrMetallicRoughness.baseColorTexture.index;
        if (tex_idx < 0 || tex_idx >= (int)gltf.textures.size()) {
            return nullptr;
        }
        // step 2: which IMAGE does that texture point to?
        int img_idx = gltf.textures[tex_idx].source;
        if (img_idx < 0 || img_idx >= (int)loaded_textures.size()) {
            return nullptr;
        }
        // step 3: return the engine texture we created for that image
        return loaded_textures[img_idx];
    }


    std::shared_ptr<bige::texture> resolve_normal_texture(const tinygltf::Model& gltf, const tinygltf::Material& mat, const std::vector<std::shared_ptr<bige::texture>>& loaded_textures) {
        _log("resolving normal texture");
        // step 1: which TEXTURE does the material's normalTexture point to?
        int tex_idx = mat.normalTexture.index;
        if (tex_idx < 0 || tex_idx >= (int)gltf.textures.size()) {
            return nullptr;
        }
        // step 2: which IMAGE does that texture point to?
        int img_idx = gltf.textures[tex_idx].source;
        if (img_idx < 0 || img_idx >= (int)loaded_textures.size()) {
            return nullptr;
        }
        // step 3: return the engine texture we created for that image
        return loaded_textures[img_idx];
    }

    std::shared_ptr<bige::texture> resolve_metallic_roughness_texture(const tinygltf::Model& gltf, const tinygltf::Material& mat, const std::vector<std::shared_ptr<bige::texture>>& loaded_textures) {
        _log("resolving normal texture");
        // step 1: which TEXTURE does the material's normalTexture point to?
        int tex_idx = mat.pbrMetallicRoughness.metallicRoughnessTexture.index;
        if (tex_idx < 0 || tex_idx >= (int)gltf.textures.size()) {
            return nullptr;
        }
        // step 2: which IMAGE does that texture point to?
        int img_idx = gltf.textures[tex_idx].source;
        if (img_idx < 0 || img_idx >= (int)loaded_textures.size()) {
            return nullptr;
        }
        // step 3: return the engine texture we created for that image
        return loaded_textures[img_idx];
    }

    void gltf_loader::traverse_nodes(const tinygltf::Model& gltf, int node_idx, const glm::mat4& parent_transform, model& result, const std::vector<std::shared_ptr<bige::texture>>& loaded_textures) {
        const auto& node = gltf.nodes[node_idx];

        // Build this node's local transform
        glm::mat4 local = glm::mat4(1.0f);

        if (!node.matrix.empty()) {
            // glTF matrix is column-major, same as glm
            local = glm::make_mat4(node.matrix.data());
        }
        else {
            // TRS components (apply in reverse: T * R * S)
            glm::vec3 t(0.0f), s(1.0f);
            glm::quat r(1.0f, 0.0f, 0.0f, 0.0f);

            if (!node.translation.empty())
                t = glm::vec3((float)node.translation[0], (float)node.translation[1], (float)node.translation[2]);
            if (!node.rotation.empty())
                // glTF quaternion is [x, y, z, w]
                r = glm::quat((float)node.rotation[3], (float)node.rotation[0], (float)node.rotation[1], (float)node.rotation[2]);
            if (!node.scale.empty())
                s = glm::vec3((float)node.scale[0], (float)node.scale[1], (float)node.scale[2]);

            local = glm::translate(glm::mat4(1.0f), t)
                * glm::mat4_cast(r)
                * glm::scale(glm::mat4(1.0f), s);
        }

        glm::mat4 world = parent_transform * local;

        // If this node has a mesh, build its primitives with world transform
        if (node.mesh >= 0 && node.mesh < (int)gltf.meshes.size()) {
            const auto& gltf_mesh = gltf.meshes[node.mesh];
            for (const auto& prim : gltf_mesh.primitives) {
                auto built_mesh = build_mesh(gltf, prim);
                if (!built_mesh) continue;
                built_mesh->name = gltf_mesh.name;

                auto p = std::make_unique<model::primitive>();
                p->mesh = std::move(built_mesh);
                p->transform = bige::transform(world);  // <-- attach the accumulated TRS
                if (prim.material >= 0 && prim.material < (int)gltf.materials.size()) {
                    auto base_color_tex = resolve_base_color_texture(gltf, gltf.materials[prim.material], loaded_textures);
                    auto normal_tex = resolve_normal_texture(gltf, gltf.materials[prim.material], loaded_textures);
					auto rough_metal_tex = resolve_metallic_roughness_texture(gltf, gltf.materials[prim.material], loaded_textures);
                    bige::material* mat = bige::materials::choose_material_tglf((void*)&gltf.materials[prim.material], base_color_tex, normal_tex, rough_metal_tex);
                    p->material.reset(mat);
                }
                else {
                    p->material.reset(new bige::materials::solid_color_material(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f)));
                }
                result.add_primitive(std::move(p));
            }
        }

        // Recurse into children
        for (int child_idx : node.children)
            traverse_nodes(gltf, child_idx, world, result, loaded_textures);
    }

    std::unique_ptr<model> gltf_loader::load_from_assets(const std::string& path) {
        return load(bige::settings::assets_folder() + "\\" + path);
    }

    // ---------- top-level load ----------
    std::unique_ptr<model> gltf_loader::load(const std::string& path) {
        _log("gltf loader");

        tinygltf::TinyGLTF loader;
        tinygltf::Model gltf;
        std::string err, warn;

        bool ok = false;

        auto t0 = std::chrono::high_resolution_clock::now();
        if (path.size() >= 4 && path.compare(path.size() - 4, 4, ".glb") == 0) {
            ok = loader.LoadBinaryFromFile(&gltf, &err, &warn, path);
        }
        else {
            ok = loader.LoadASCIIFromFile(&gltf, &err, &warn, path);
        }

        auto t2 = std::chrono::high_resolution_clock::now();
        _l("loaded boom box object in {} ms", std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t0).count());

        if (!warn.empty()) { _l("warning: {}", warn); }
        if (!err.empty()) { _l("error: {}", err); }
        if (!ok) {
            _l("loading failed");
            return nullptr;
        }

        std::vector<std::shared_ptr<bige::texture>> loaded_textures;
        loaded_textures.reserve(gltf.images.size());

        for (size_t i = 0; i < gltf.images.size(); i++) {
            const auto& img = gltf.images[i];

            uint32_t signature = 0;
            if (img.image.size() >= 4) {
                signature = (img.image[0] << 24) | (img.image[1] << 16)
                    | (img.image[2] << 8) | img.image[3];
            }

            if (img.image.empty()) {
                loaded_textures.push_back(nullptr);
                continue;
            }

            auto tex = bige::texture::load_from_data(img.width, img.height, img.component, img.image.data());
            loaded_textures.push_back(tex);
        }

        auto result = std::make_unique<model>();
        const auto& scene = gltf.scenes[gltf.defaultScene >= 0 ? gltf.defaultScene : 0];
        for (int root_node : scene.nodes) {
            traverse_nodes(gltf, root_node, glm::mat4(1.0f), *result, loaded_textures);
        }
        return result;
    }

}