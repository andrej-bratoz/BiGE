#pragma once

#include <bige-scene.h>
#include "ba-objects.h"

namespace ba {
	struct default_scene : public bige::scene {
		default_scene() {
			add_object(std::move(std::make_unique<scene_object>()));
		}
	};
}