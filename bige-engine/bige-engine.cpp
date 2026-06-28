

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifdef BIGE_ENGINE_HAS_EDITOR
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
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
			m_window = std::unique_ptr<bige::Window>(new bige::Window(800, 600, title.value_str));
		}
		else {
			m_window = std::unique_ptr<bige::Window>(new bige::Window(800, 600, "BiGE Engine Window"));
		}
		
		bige::Window::width = 800;
		bige::Window::height = 600;
		if (!m_window->create()) {
			return false;
		}
		bige::Window::g_wnd = m_window.get();

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			return false;
		}
#ifdef BIGE_ENGINE_HAS_EDITOR
		ImGui::CreateContext();
		ImGui_ImplGlfw_InitForOpenGL(BIGE_WINDOW_HANDLE(m_window->handle()), true);
		ImGui_ImplOpenGL3_Init("#version 330");
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
#ifdef BIGE_ENGINE_HAS_EDITOR
		st.subscribe(bige::state::WE_AMBIENT_STRENGTH, BIGE_REGISTER_STATE_CB(state_cb::world_editor_callback, this));
		st.subscribe("direction_light.direction.azimuth.elevation", BIGE_REGISTER_STATE_CB(state_cb::world_editor_callback, this));
#endif
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

#ifdef BIGE_ENGINE_HAS_EDITOR
		float ambient_strength = 0.55f;
		float azimuth = -0.0f;
		float elevation = 45.0f;
		float speed = 10.0f;
#endif
		while (!glfwWindowShouldClose(BIGE_WINDOW_HANDLE(m_window->handle()))) {
			
#ifdef BIGE_ENGINE_HAS_EDITOR
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
#			
			bige::engine_state st;
			ImGui::Begin("Controls");
			if (ImGui::SliderFloat("Ambient light strength", &ambient_strength, 0.0f, 1.0f)) {
				engine_state_impl::state_variant var((float)ambient_strength);
				var.value_type = engine_state_impl::state_variant::type::f32;
				var.value_float = ambient_strength;
				st.state_set("direction_light.ambient_strength", var, true);
			}

			if (ImGui::SliderFloat("Azimuth", &azimuth, -180.0f, 180.0f)) {
				engine_state_impl::state_variant var((float)azimuth);
				var.value_type = engine_state_impl::state_variant::type::vec2;
				var.value_vec2 = { azimuth, elevation };
				st.state_set("direction_light.direction.azimuth.elevation", var, true);
			}
			if (ImGui::SliderFloat("Elevation", &elevation, -90.0f, 90.0f)) {
				engine_state_impl::state_variant var((float)elevation);
				var.value_type = engine_state_impl::state_variant::type::vec2;
				var.value_vec2 = { azimuth, elevation };
				st.state_set("direction_light.direction.azimuth.elevation", var, true);
			}
			ImGui::SliderFloat("FarPlane", &orbit_camera::instance->far_plane, 100.0f, 100000.0f);
			ImGui::SliderFloat("NearPlane", &orbit_camera::instance->near_plane, 0.001f, 1.0f);

			if (ImGui::SliderFloat("Speed Multiplier:", &speed, 0.01f, 100.0f)) {}

			ImGui::Text("Ambient strength = %f", bige::scene::m_sun->ambient_strength);
			ImGui::Text("Ambient light direction [ %f, %f, %f ]", bige::scene::m_sun->direction.x, bige::scene::m_sun->direction.y, bige::scene::m_sun->direction.z);
			ImGui::End();
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
			if(glfwGetKey((GLFWwindow*)m_window->handle(), GLFW_KEY_E) == GLFW_PRESS) bige::orbit_camera::instance->move(bige::OrbitMovement::FORWARD, m_delta_time * 30);
			if (glfwGetKey((GLFWwindow*)m_window->handle(), GLFW_KEY_R) == GLFW_PRESS) bige::orbit_camera::instance->move(bige::OrbitMovement::BACKWARD, m_delta_time * 30);

			if (glfwGetKey((GLFWwindow*) m_window->handle(), GLFW_KEY_W) == GLFW_PRESS) bige::orbit_camera::instance->move(bige::OrbitMovement::FORWARD, m_delta_time* speed);
			if (glfwGetKey((GLFWwindow*)m_window->handle(), GLFW_KEY_S) == GLFW_PRESS) bige::orbit_camera::instance->move(bige::OrbitMovement::BACKWARD, m_delta_time* speed);
			if (glfwGetKey((GLFWwindow*)m_window->handle(), GLFW_KEY_A) == GLFW_PRESS) bige::orbit_camera::instance->move(bige::OrbitMovement::LEFT, m_delta_time* speed);
			if (glfwGetKey((GLFWwindow*)m_window->handle(), GLFW_KEY_D) == GLFW_PRESS) bige::orbit_camera::instance->move(bige::OrbitMovement::RIGHT, m_delta_time* speed);
#endif
			//glClear(GL_COLOR_BUFFER_BIT);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			// render objects

			if (m_scene) {
				for (const auto& obj : m_scene->objects_ref()) {
					if (!obj) continue;
					if (obj->is_dead()) continue;
					obj->dispatch(bige::objects::object::dispatch_call_id::prepare, this);
					//obj->dispatch(bige::objects::object::dispatch_call_id::set_uniforms,this);
					obj->dispatch(bige::objects::object::dispatch_call_id::draw, this);
					obj->dispatch(bige::objects::object::dispatch_call_id::move, this);
					obj->dispatch(bige::objects::object::dispatch_call_id::animate, this);
				}
			}
#ifdef BIGE_ENGINE_HAS_EDITOR
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
			glfwSwapBuffers(BIGE_WINDOW_HANDLE(m_window->handle()));
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