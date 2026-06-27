#pragma once
#include <bige-state_cb.h>
#include <bige-scene.h>
#include <glm/vec3.hpp>
#include <memory>

namespace biwe {
	
	struct world_state {
		constexpr static const char* SCENE_DIRECTION_LIGHT_DIRECTION = "direction_light.direction";
		constexpr static const char* SCENE_DIRECTION_LIGHT_LIGHT_COLOR = "direction_light.light_color";
		constexpr static const char* SCENE_DIRECTION_LIGHT_AMBIENT_COLOR = "direction_light.ambient_color";
		constexpr static const char* SCENE_DIRECTION_LIGHT_AMBIENT_STRENGTH = "direction_light.ambient_strength";

		world_state() {
			state.state_set(SCENE_DIRECTION_LIGHT_DIRECTION,
				BIGE_STATE_VEC3(bige::scene::m_sun->direction.x, bige::scene::m_sun->direction.y, bige::scene::m_sun->direction.z), false);

			state.state_set(SCENE_DIRECTION_LIGHT_LIGHT_COLOR,
				BIGE_STATE_VEC3(bige::scene::m_sun->light_color.x, bige::scene::m_sun->light_color.y, bige::scene::m_sun->light_color.z), false);

			state.state_set(SCENE_DIRECTION_LIGHT_AMBIENT_COLOR,
				BIGE_STATE_VEC3(bige::scene::m_sun->ambient_color.x, bige::scene::m_sun->ambient_color.y, bige::scene::m_sun->ambient_color.z), false);

			state.state_set(SCENE_DIRECTION_LIGHT_AMBIENT_STRENGTH,
				BIGE_STATE_FLOAT(bige::scene::m_sun->ambient_strength), false);
		}

		glm::vec3 light_direction() {
			auto val = state.state_get(SCENE_DIRECTION_LIGHT_DIRECTION);
			if (BIGE_CHECK_STATE_VEC3(val)) {
				return glm::vec3(val.value_vec3.x, val.value_vec3.y, val.value_vec3.z);
			}
		}

		void light_direction(const glm::vec3& dir) {
			state.state_set(SCENE_DIRECTION_LIGHT_DIRECTION, BIGE_STATE_VEC3(dir.x, dir.y, dir.z));
		}

		glm::vec3 light_color() {
			auto val = state.state_get(SCENE_DIRECTION_LIGHT_LIGHT_COLOR);
			if (BIGE_CHECK_STATE_VEC3(val)) {
				return glm::vec3(val.value_vec3.x, val.value_vec3.y, val.value_vec3.z);
			}
		}

		void light_color(const glm::vec3& color) {
			state.state_set(SCENE_DIRECTION_LIGHT_LIGHT_COLOR, BIGE_STATE_VEC3(color.x, color.y, color.z));
		}

		glm::vec3 ambient_color() {
			auto val = state.state_get(SCENE_DIRECTION_LIGHT_AMBIENT_COLOR);
			if (BIGE_CHECK_STATE_VEC3(val)) {
				return glm::vec3(val.value_vec3.x, val.value_vec3.y, val.value_vec3.z);
			}
		}

		void ambient_color(const glm::vec3& color) {
			state.state_set(SCENE_DIRECTION_LIGHT_AMBIENT_COLOR, BIGE_STATE_VEC3(color.x, color.y, color.z));
		}

		float ambient_strength() {
			auto val = state.state_get(SCENE_DIRECTION_LIGHT_AMBIENT_STRENGTH);
			if (BIGE_CHECK_STATE_FLOAT(val)) {
				return val.value_float;
			}
		}

		void ambient_strength(float strength) {
			state.state_set(SCENE_DIRECTION_LIGHT_AMBIENT_STRENGTH, BIGE_STATE_FLOAT(strength));
		}

		bige::engine_state state;

		inline static std::unique_ptr<world_state> instance = std::make_unique<world_state>();
	};

	struct world_scene : bige::scene {
		// add any editor specific scene data or functionality here
		inline static std::unique_ptr<world_scene> instance = std::make_unique<world_scene>();
	};

	struct world_editor_window {
		// read from world-editor.rc
		void show() {
			
		}
	};
}