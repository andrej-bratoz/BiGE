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
		auto obj = loader.load(bige::settings::assets_folder() + "\\cs_office_csgo_real_light_version_4k.glb");
		attach_model(std::move(obj));
	}
}
