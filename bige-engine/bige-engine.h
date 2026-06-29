#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>


#include <bige-window.h>
#include <bige-utils.h>
#include <bige-object.h>
#include <bige-shadercache.h>
#include <bige-scene.h>
#include <bige-camera.h>
#ifdef BIGE_ENGINE_HAS_JSON
#	include <nlohmann/json.hpp>
#endif
#include <memory>
#include <map>
#include <vector>
#include <atomic>
#include <thread>
#include <mutex>
#include <random>

namespace bige {

#	define BIGE_RGBA_COLOR_NORMALIZED(r,g,b,a) ((float)(r)/255.0f), ((float)(g)/255.0f), ((float)(b)/255.0f), ((float)(a)/255.0f)
#	define BIGE_RGB_COLOR_NORMALIZED(r,g,b,a) ((float)(r)/255.0f), ((float)(g)/255.0f), ((float)(b)/255.0f)
#	define BIGE_RGBA_COLOR(r,g,b,a) ((float)(r)), ((float)(g)), ((float)(b)), ((float)(a))
#	define BIGE_RGB_COLOR(r,g,b) ((float)(r)), ((float)(g)), ((float)(b))

	struct engine_state_impl {
		struct state_variant {
#ifdef BIGE_ENGINE_HAS_JSON
			typedef nlohmann::json json;
#endif

			struct vec2 {
				float x, y;
			};
			
			struct vec3 {
				float x, y, z;
			};
			
			struct vec4 {
				float x, y, z, w;
			};

			enum class type {
				none,
				i32,
				f32,
				str,
				vec2,
				vec3,
				vec4,
#ifdef BIGE_ENGINE_HAS_JSON
				json,
#endif
			};
			std::string value_str;
#ifdef BIGE_ENGINE_HAS_JSON
			json value_json;
#endif
			vec2 value_vec2{ 0.0f, 0.0f };
			vec3 value_vec3{ 0.0f, 0.0f, 0.0f };
			vec4 value_vec4 { 0.0f, 0.0f, 0.0f, 0.0f };
			int32_t value_int = 0;
			float value_float = 0;
			state_variant() : value_type(type::none) {}
			state_variant(int val) : value_int(val), value_type(type::i32) {}
			state_variant(float val) : value_float(val), value_type(type::f32) {
				int i = 0;
			
			}
			state_variant(const std::string& val) : value_str(val), value_type(type::str) {}
			state_variant(const char* val) : value_str(val), value_type(type::str) {}
#ifdef BIGE_ENGINE_HAS_JSON
			state_variant(const json& val) : value_json(val), value_type(type::json) {}
#endif
			state_variant(const vec2& val) : value_vec2({ val.x, val.y }), value_type(type::vec2) {}
			state_variant(const vec3& val) : value_vec3({ val.x, val.y, val.z }), value_type(type::vec3) {}
			state_variant(const vec4& val) : value_vec4({ val.x, val.y, val.z, val.w }), value_type(type::vec4) {}
			type vt() const { return value_type; }
#			define BIGE_CHECK_STATE_VEC2(x) (x.vt() == bige::engine_state_impl::state_variant::type::vec2)
# 			define BIGE_CHECK_STATE_VEC3(x) (x.vt() == bige::engine_state_impl::state_variant::type::vec3)
# 			define BIGE_CHECK_STATE_VEC4(x) (x.vt() == bige::engine_state_impl::state_variant::type::vec4)
# 			define BIGE_CHECK_STATE_INT32(x) (x.vt() == bige::engine_state_impl::state_variant::type::i32)
# 			define BIGE_CHECK_STATE_FLOAT(x) (x.vt() == bige::engine_state_impl::state_variant::type::f32)
# 			define BIGE_CHECK_STATE_STRING(x) (x.vt() == bige::engine_state_impl::state_variant::type::str)
#ifdef BIGE_ENGINE_HAS_JSON
# 			define BIGE_CHECK_STATE_JSON(x) (x.vt() == bige::engine_state_impl::state_variant::type::json)
#endif
# 			define BIGE_CHECK_STATE_NONE(x) (x.vt() == bige::engine_state_impl::state_variant::type::none)
#			define BIGE_STATE_VEC2(r,g) bige::engine_state_impl::state_variant(bige::engine_state_impl::state_variant::vec2{ r, g })
#			define BIGE_STATE_VEC3(r,g,b) bige::engine_state_impl::state_variant(bige::engine_state_impl::state_variant::vec3{ r, g, b })
#			define BIGE_STATE_VEC4(r,g,b,alpha) bige::engine_state_impl::state_variant(bige::engine_state_impl::state_variant::vec4{ r, g, b, alpha }) 
#ifdef BIGE_ENGINE_HAS_JSON
#			define BIGE_STATE_JSON(...) bige::engine_state_impl::state_variant(bige::engine_state_impl::state_variant::json({ __VA_ARGS__ }))
#endif
#			define BIGE_STATE_INT32(val) bige::engine_state_impl::state_variant((int)val)
#			define BIGE_STATE_FLOAT(val) bige::engine_state_impl::state_variant((float)val)
#			define BIGE_STATE_STRING(val) bige::engine_state_impl::state_variant((std::string)val)
#			define BIGE_STATE_NONE(val) bige::engine_state_impl::state_variant()

			type value_type;

		private:
		};
		
		struct state_cb {
			virtual void operator()(const std::string& key, const state_variant& value) = 0;
			virtual bool is_async() const { return false; }
		};

		void state_set(const std::string& key, const state_variant& value, bool notify = true) {
			m_states[key] = value;
			if (notify) {
				state_call(key, value);
			}
		}

		void state_call(const std::string& key, const state_variant& value) {
			if (m_callbacks.find(key) != m_callbacks.end()) {
				for (const auto& cb : m_callbacks[key]) {
					if (cb->is_async()) { // use to launch independent long running tasks without blocking the main thread
						std::thread t([key, value, cb] {
							(*cb)(key, value);
						});
						t.detach();
					}
					else {
						(*cb)(key, value);
					}
				}
			}
		}
		state_variant state_get(const std::string& key) {
			if (m_states.find(key) != m_states.end()) {
				return m_states[key];
			}
			return state_variant();
		}

		void state_del(const std::string& key) {
			m_states.erase(key);
		}

		void subscribe(const std::string& key, std::shared_ptr<state_cb> callback) {
			m_callbacks[key].push_back(callback);
		}

		void destroy() {
			m_states.clear();
			m_callbacks.clear();
		}

	private:
		std::map<std::string, state_variant> m_states;
		std::map<std::string, std::vector<std::shared_ptr<state_cb>>> m_callbacks;
	};
#ifdef BIGE_ENGINE_HAS_JSON
	typedef bige::engine_state_impl::state_variant::json json;
#endif

#	define BIGE_ENGINE_STATE_ASNYC_CB bool is_async() const override { return true; }
#	define BIGE_ENGINE_STATE_SNYC_CB bool is_async() const override { return false; }
#	define BIGE_ENGINE_STATE_CALL_IMPL(key, value) void operator()(const std::string& key, const bige::engine_state_impl::state_variant& value) override
#	define BIGE_ENGINE_STATE_CB(x) struct x : bige::engine_state_impl::state_cb
#	define BIGE_ENGINE_DECLARE_STATE_CB(type) struct type : bige::engine_state_impl::state_cb { \
												virtual void operator()(const std::string& key, const bige::engine_state_impl::state_variant& value) override;\
												BIGE_ENGINE_STATE_SNYC_CB\
											  }

#	define BIGE_ENGINE_DECLARE_STATE_PARAM_CB(type, pt, pn) struct type : bige::engine_state_impl::state_cb { \
												type(pt pn) : m_##pn(pn) {} \
												virtual void operator()(const std::string& key, const bige::engine_state_impl::state_variant& value) override;\
												BIGE_ENGINE_STATE_SNYC_CB \
												pt m_##pn;				  \
											  }

#	define BIGE_ENGINE_DECLARE_ASYNC_STATE_CB(type) struct type : bige::engine_state_impl::state_cb { \
												virtual void operator()(const std::string& key, const bige::engine_state_impl::state_variant& value) override; \
												BIGE_ENGINE_STATE_ASNYC_CB \
											  }

#	define BIGE_REGISTER_STATE_CB(type, ...) std::make_shared<type>(__VA_ARGS__)




	struct engine {
		engine() {}
		bool initialize();
		void run();
		void shutdown();
		void set_scene(std::shared_ptr<bige::scene> scene);
		float delta_time() const;
		void register_state_callbacks();

	private:
		std::shared_ptr<bige::scene> m_scene;
		float m_last_time = 0.0f;
		float m_delta_time = 0.0f;
		int m_frame_count = 0;
	};
	struct engine_state {
		void state_set(const std::string& key, const engine_state_impl::state_variant& value, bool notify = true) {
			s_state->state_set(key, value, notify); 
		}
		void state_call(const std::string& key, const engine_state_impl::state_variant& value) {
			s_state->state_call(key, value); 
		}
		engine_state_impl::state_variant state_get(const std::string& key) {
			return s_state->state_get(key);
		}
		void state_del(const std::string& key) {
			return s_state->state_del(key);
		}
		void subscribe(const std::string& key, std::shared_ptr<engine_state_impl::state_cb> callback) {
			s_state->subscribe(key, callback);
		}
		friend struct bige::engine;
	private:
		inline static std::shared_ptr<engine_state_impl> s_state = std::make_shared<engine_state_impl>();
	};
}