
#include <bige-logging.h>
#include <bige-window.h>
#include <bige-state_cb.h>
#include <Windows.h>

namespace bige {
	void state_cb::trace_logger::operator()(const std::string& key, const bige::engine_state_impl::state_variant& value) {
		if (!(value.vt() == bige::engine_state_impl::state_variant::type::str)) {
			return;
		}
	}

	void state_cb::message_box::operator()(const std::string& key, const engine_state_impl::state_variant& value) {
#ifdef BIGE_ENGINE_HAS_JSON
		if (value.vt() == engine_state_impl::state_variant::type::json) {
			bige::json j = value.value_json;
			std::string title = j.contains("title") ? j["title"].get<std::string>() : "BiGe Engine Notification";
			std::string message = j.contains("message") ? j["message"].get<std::string>() : "";
			UINT type = j.contains("type") ? j["type"].get<int>() : MB_OK;
			MessageBoxA(NULL, message.c_str(), title.c_str(), type);
		}
		else if (value.vt() == engine_state_impl::state_variant::type::str) {
			MessageBoxA(NULL, value.value_str.c_str(), "BiGe Engine Notification", MB_OK);
		}
#else
		if (value.vt() == engine_state_impl::state_variant::type::str) {
			MessageBoxA(NULL, value.value_str.c_str(), "BiGe Engine Notification", MB_OK);
		}
#endif
	}

	void state_cb::window_color_callback::operator()(const std::string& key, const engine_state_impl::state_variant& value) {
		if (BIGE_CHECK_STATE_VEC4(value)) {
			m_engine->window().set_background_color(BIGE_RGBA_COLOR_NORMALIZED(value.value_vec4.x, value.value_vec4.y, value.value_vec4.z, value.value_vec4.w));
		}
	}

	void state_cb::world_editor_callback::operator()(const std::string& key, const engine_state_impl::state_variant& value) {
		_log("world_editor_callback");
		if(key == "direction_light.light_color") {
			if(BIGE_CHECK_STATE_VEC3(value)) {
				bige::scene::m_sun->light_color = glm::vec3(value.value_vec3.x, value.value_vec3.y, value.value_vec3.z);
			}
		} else if(key == "direction_light.ambient_color") {
			if(BIGE_CHECK_STATE_VEC3(value)) {
				bige::scene::m_sun->ambient_color = glm::vec3(value.value_vec3.x, value.value_vec3.y, value.value_vec3.z);
			}
		} else if(key == "direction_light.ambient_strength") {
			if ((value.vt() == bige::engine_state_impl::state_variant::type::f32)) {
				bige::scene::m_sun->ambient_strength = value.value_float;
				_l("world_editor_callback: ambient_strength = {}", std::to_string(value.value_float));
			}
		} else if (key == "direction_light.direction.azimuth.elevation") {
			if ((value.vt() == bige::engine_state_impl::state_variant::type::vec2)) {
				float azRad = glm::radians(value.value_vec2.x);
				float elRad = glm::radians(value.value_vec2.y);
				bige::scene::m_sun->direction = glm::normalize(glm::vec3(
					cos(elRad) * sin(azRad),
					sin(elRad),
					cos(elRad) * cos(azRad)
				));
			}
		}
		
	}
}