

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifdef BIGE_ENGINE_HAS_EDITOR
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <we-input-events.h>
#endif

//#include <Windows.h>
#include <bige-engine.h>
#include <bige-logging.h>
#include <bige-state_cb.h>
#include <execution>
namespace bige {

	bool engine::initialize() {
		glfwInit();
		bige::engine_state st;
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		register_state_callbacks();

		auto title = st.state_get("engine.window.title");
		if (BIGE_CHECK_STATE_STRING(title)) {
			bige::Window::engine_window = std::unique_ptr<bige::Window>(new bige::Window(800, 600, title.value_str));
		}
		else {
			bige::Window::engine_window = std::unique_ptr<bige::Window>(new bige::Window(800, 600, "BiGE Engine Window"));
		}
		
		if (!bige::Window::engine_window->create()) {
			return false;
		}

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			return false;
		}
#ifdef BIGE_ENGINE_HAS_EDITOR
		we::editor_gui gui;
		gui.prepare(BIGE_WINDOW_HANDLE(bige::Window::engine_window->handle()));
#endif
		st.state_set(bige::state::SYSTEM_UPTIME_START, (int)glfwGetTime(), false);
		return true;
	}

	void engine::set_scene(std::shared_ptr<bige::scene> scene) {
		m_scene = scene;
	}

	float engine::delta_time() const { return 0; }

	void engine::register_state_callbacks() {
		bige::engine_state st;
		st.subscribe(bige::state::MESSAGE_CB_SHOW, BIGE_REGISTER_STATE_CB(state_cb::message_box));
		st.subscribe(bige::state::WINDOW_BACKGROUND_COLOR, BIGE_REGISTER_STATE_CB(bige::state_cb::window_color_callback, this));
	}

	void engine::run() {
		bige::engine_state st;
		m_last_time = (float)glfwGetTime();
		m_delta_time = 0;
		_log("engine");
		_lt("entering main loop");

		float last_frame = 0;

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW); // adjust if your mesh winding is clockwise -- see note below
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		while (!glfwWindowShouldClose(BIGE_WINDOW_HANDLE(bige::Window::engine_window->handle()))) {
			
#ifdef BIGE_ENGINE_HAS_EDITOR
			we::editor_gui gui;
			gui.set_gui();
#endif

			float currentFrame = static_cast<float>(glfwGetTime());
			m_delta_time = currentFrame - last_frame;
			last_frame = currentFrame;

			m_frame_count++;
			if (m_delta_time >= 1.0) {
				st.state_set(bige::state::ENGINE_FPS, BIGE_STATE_FLOAT(m_frame_count), false);
				m_frame_count = 0;
			}
			glfwPollEvents();

#ifdef BIGE_ENGINE_HAS_EDITOR
			we::world_scene_handle_movement((GLFWwindow*)bige::Window::engine_window->handle(), m_delta_time, bige::orbit_camera::instance->speed_multiplier);
#endif
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			if (m_scene) {
				for (const auto& obj : m_scene->objects_ref()) {
					if (!obj) continue;
					if (obj->is_dead()) continue;
					obj->dispatch(bige::objects::object::dispatch_call_id::prepare, this);
					obj->dispatch(bige::objects::object::dispatch_call_id::draw, this);
					obj->dispatch(bige::objects::object::dispatch_call_id::move, this);
					obj->dispatch(bige::objects::object::dispatch_call_id::animate, this);
				}
			}
#ifdef BIGE_ENGINE_HAS_EDITOR
			gui.render();
#endif
			glfwSwapBuffers((GLFWwindow*)bige::Window::engine_window->handle());
		}
		if (m_scene) {
			for (const auto& obj : m_scene->objects_ref()) {
				obj->dispatch(bige::objects::object::dispatch_call_id::teardown, this);
			}
		}
	}
	
	void engine::shutdown() {
		bige::engine_state::s_state->destroy();
		glfwTerminate();
	}
}