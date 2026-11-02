#include "editor-scene.h"
#include <bige-settings.h>

namespace biwe {

	world_scene::world_scene() {
		add_object(std::move(std::make_unique<scene_object>()));
	}

	scene_object::scene_object() {
		bige::gltf_loader loader;
		//	auto obj = loader.load_from_assets("greek_temple_scan.glb");
		//	attach_model(std::move(obj));
	}

}