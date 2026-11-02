#pragma once

#include <bige-scene.h>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <map>

namespace we {
	void world_scene_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	void world_scene_cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
	void world_scene_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	struct register_editor_input_events {
		register_editor_input_events(GLFWwindow* window) {
			glfwSetMouseButtonCallback(window, world_scene_mouse_button_callback);
			glfwSetCursorPosCallback(window, world_scene_cursor_pos_callback);
			glfwSetScrollCallback(window, world_scene_scroll_callback);
		}
	};

	void world_scene_handle_movement(GLFWwindow* window, float delta_time, float speed);

	struct editor_gui {
		void prepare(GLFWwindow* wnd);
		void set_gui(bige::scene* s);
		void render() {
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}
		bool load_tree_view = true;
		std::map<std::string, std::string> m_map_original_adjusted_names;
	};

}