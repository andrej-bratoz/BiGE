#include "editor-scene.h"
#include <bige-settings.h>

namespace biwe {

	world_scene::world_scene() {
		add_object(std::move(std::make_unique<scene_object>()));
	}

	scene_object::scene_object() {
		bige::gltf_loader loader;
		auto obj = loader.load(bige::settings::assets_folder() + "\\cs_office_csgo_real_light_version.glb", [](const tinygltf::Material* mat,
			std::shared_ptr<bige::texture> tex, std::shared_ptr<bige::texture> normal_tex, std::shared_ptr<bige::texture> rough_met) {
				std::unique_ptr<bige::material> material = std::make_unique<bige::materials::solid_color_material>(glm::vec4(1.0f));
				if (!mat || (!tex && !normal_tex)) {
					return material;
				}

				material = std::make_unique<bige::materials::pbr_material>();
				material->base_color_texture = tex;
				material->normal_texture = normal_tex;
				material->metallic_roughness_texture = rough_met;
				return material;
			});
		attach_model(std::move(obj));
	}

}