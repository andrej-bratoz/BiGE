#include <bige-camera.h>
#include <bige-state_cb.h>
#include "we-input-events.h"
#include "we-state.h"

namespace we {
	void world_scene_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
		if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) { return; }
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			we::editor_state::instance->m_rotating = (action == GLFW_PRESS);
			if (we::editor_state::instance->m_rotating) {
				glfwGetCursorPos(window, &we::editor_state::instance->m_last_x, &we::editor_state::instance->m_last_y);
			}
		}
		else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
			we::editor_state::instance->m_panning = (action == GLFW_PRESS);
			if (we::editor_state::instance->m_panning) {
				glfwGetCursorPos(window, &we::editor_state::instance->m_last_x, &we::editor_state::instance->m_last_y);
			}
		}
	}

	void world_scene_cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
		if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) { return; }
		float xoffset = static_cast<float>(xpos - we::editor_state::instance->m_last_x);
		float yoffset = static_cast<float>(ypos - we::editor_state::instance->m_last_y);
		we::editor_state::instance->m_last_x = xpos;
		we::editor_state::instance->m_last_y = ypos;

		if (we::editor_state::instance->m_rotating) {
			bige::orbit_camera::instance->rotate(xoffset, -yoffset);
		}
		if (we::editor_state::instance->m_panning) {
			bige::orbit_camera::instance->pan(xoffset, yoffset);
		}
	}

	void world_scene_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
		if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) { return; }
		bige::orbit_camera::instance->zoom((float)yoffset);
	}

	void world_scene_handle_movement(GLFWwindow* window, float delta_time, float speed) {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) bige::orbit_camera::instance->move(bige::OrbitMovement::FORWARD, delta_time * speed);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) bige::orbit_camera::instance->move(bige::OrbitMovement::BACKWARD, delta_time * speed);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) bige::orbit_camera::instance->move(bige::OrbitMovement::LEFT, delta_time * speed);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) bige::orbit_camera::instance->move(bige::OrbitMovement::RIGHT, delta_time * speed);
	}

	void editor_gui::prepare(GLFWwindow* wnd) {
		ImGui::CreateContext();
		ImGui_ImplGlfw_InitForOpenGL(wnd, true);
		ImGui_ImplOpenGL3_Init("#version 330");
	}

	void editor_gui::set_gui() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		bige::engine_state st;

		ImGui::Begin("Controls");
		ImGui::SliderFloat("Ambient light strength", &bige::scene::m_sun->ambient_strength, 0.0f, 1.0f);

		if (ImGui::SliderFloat("Azimuth", &bige::scene::m_sun->azimuth, -180.0f, 180.0f)) {
			float azRad = glm::radians(bige::scene::m_sun->azimuth);
			float elRad = glm::radians(bige::scene::m_sun->elevation);
			bige::scene::m_sun->direction = glm::normalize(glm::vec3(
				cos(elRad) * sin(azRad),
				sin(elRad),
				cos(elRad) * cos(azRad)
			));
		}
		if (ImGui::SliderFloat("Elevation", &bige::scene::m_sun->elevation, -90.0f, 90.0f)) {
			float azRad = glm::radians(bige::scene::m_sun->azimuth);
			float elRad = glm::radians(bige::scene::m_sun->elevation);
			bige::scene::m_sun->direction = glm::normalize(glm::vec3(
				cos(elRad) * sin(azRad),
				sin(elRad),
				cos(elRad) * cos(azRad)
			));
		}
		ImGui::SliderFloat("FarPlane", &bige::orbit_camera::instance->far_plane, 10.0f, 100000.0f);
		ImGui::SliderFloat("NearPlane", &bige::orbit_camera::instance->near_plane, 0.001f, 5.0f);
		ImGui::SliderFloat("Speed Multiplier:", &bige::orbit_camera::instance->speed_multiplier, 0.01f, 1000.0f);

		ImGui::Text("Ambient strength = %f", bige::scene::m_sun->ambient_strength);
		ImGui::Text("Ambient light direction [ %f, %f, %f ]", bige::scene::m_sun->direction.x, bige::scene::m_sun->direction.y, bige::scene::m_sun->direction.z);
		ImGui::End();

	}

}