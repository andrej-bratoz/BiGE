#include <memory>

#include <bige-utils.h>
#include <bige-settings.h>
#include <bige-texture.h>
#include <bige-logging.h>
#include <materials/bige-materials.h>
#include <bige-gltf-parser.h>
#include "ba-objects.h"

namespace ba {
	scene_object::scene_object() {
		bige::gltf_loader loader;
		auto obj = loader.load(bige::settings::assets_folder() + "\\temple_of_apollo_-_naxos_greece_cyark_dataset.glb", [](const tinygltf::Material* mat, 
			std::shared_ptr<bige::texture> tex, std::shared_ptr<bige::texture> normal_tex, std::shared_ptr<bige::texture> rough_met) {
			std::unique_ptr<bige::material> material = std::make_unique<bige::materials::solid_color_material>(glm::vec4(1.0f));
			if (!mat || (!tex && !normal_tex)) {
				return material;
			}
			//
			if (normal_tex) {
				material = std::make_unique<bige::materials::lit_material_normals>();
			} 
			if (tex) {
				material = std::make_unique<bige::materials::lit_material>();
			} 
			if(rough_met) {
				material = std::make_unique<bige::materials::pbr_material>();
				float metallic_factor = static_cast<float>(mat->pbrMetallicRoughness.metallicFactor);  // default 1.0 per spec
				float roughness_factor = static_cast<float>(mat->pbrMetallicRoughness.roughnessFactor); // default 1.0 per spec

				auto x = (bige::materials::pbr_material*)material.get();
				x->metallic_factor = metallic_factor;
				x->roughness_factor = roughness_factor;
			}

			if(mat->alphaMode == "MASK") {
				if (auto* lit_mat = dynamic_cast<bige::materials::lit_material*>(material.get())) {
					lit_mat->alpha_cutoff = (float)mat->alphaCutoff;
				}
				else if (auto* lit_mat_normals = dynamic_cast<bige::materials::lit_material_normals*>(material.get())) {
					lit_mat_normals->alpha_cutoff = (float)mat->alphaCutoff;
				}
			} else if(mat->alphaMode == "BLEND") {
				// for simplicity, treat BLEND materials as having the same alpha cutoff as MASK materials -- in a real engine, you'd want to handle this differently (e.g. by using a different shader and/or render state)
				if (auto* lit_mat = dynamic_cast<bige::materials::lit_material*>(material.get())) {
					lit_mat->alpha_cutoff = 0.5f; // or some other default cutoff for blended materials
				}
				else if (auto* lit_mat_normals = dynamic_cast<bige::materials::lit_material_normals*>(material.get())) {
					lit_mat_normals->alpha_cutoff = 0.5f;
				}
			}
			else {
				if (auto* lit_mat = dynamic_cast<bige::materials::lit_material*>(material.get())) {
					lit_mat->alpha_cutoff = 0.0f; // or some other default cutoff for blended materials
				}
				else if (auto* lit_mat_normals = dynamic_cast<bige::materials::lit_material_normals*>(material.get())) {
					lit_mat_normals->alpha_cutoff = 0.0f;
				}
			}

			material->base_color_texture = tex;
			material->normal_texture = normal_tex;
			material->metallic_roughness_texture = rough_met;
			return material;
		});
		attach_model(std::move(obj));
	}
}
