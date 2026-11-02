#include <bige-camera.h>
#include <bige-state_cb.h>
#include <bige-scene.h>
#include <bige-camera.h>
#include <nlohmann/json.hpp>
#include "we-input-events.h"
#include "we-state.h"
#include "editor-scene.h"
#ifdef WIN32
#	include <Windows.h>
#endif
#include <bige-gltf-parser.h>

namespace we {
	char save_as[MAX_PATH]{};
	char gltf_file[MAX_PATH]{};
	void world_scene_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
		if (bige::Window::engine_window->m_block_events) return;
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
		if (bige::Window::engine_window->m_block_events) return;
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
		if (bige::Window::engine_window->m_block_events) return;
		if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) { return; }
		bige::orbit_camera::instance->zoom((float)yoffset);
	}

	void world_scene_handle_movement(GLFWwindow* window, float delta_time, float speed) {
		if (bige::Window::engine_window->m_block_events) return;

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

	void editor_gui::set_gui(bige::scene* s) {
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
		
		auto camera_pos = bige::orbit_camera::instance->get_position();
		ImGui::Text("Camera position [ %f, %f, %f ]", camera_pos.x, camera_pos.y, camera_pos.z);

		static int viewPortx = 0;
		static int viewPorty = 0;
		if (ImGui::SliderInt("Viewport X", &viewPortx, 0, bige::Window::engine_window->m_width)) {
			glViewport(viewPortx, viewPorty, bige::Window::engine_window->m_width, bige::Window::engine_window->m_height);
		}

		if (ImGui::SliderInt("Viewport Y", &viewPorty, 0, bige::Window::engine_window->m_height)) {
			glViewport(viewPortx, viewPorty, bige::Window::engine_window->m_width, bige::Window::engine_window->m_height);
		}
		ImGui::Checkbox("Block events", &bige::Window::engine_window->m_block_events);

		{
			if (ImGui::Button("... (Add GLB Object)")) {
				char szFile[MAX_PATH] = "";
				OPENFILENAME ofn = {};
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = nullptr;
				ofn.lpstrFile = szFile;
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrFilter = "Scene Files (*.glb)\0*.glb\0All Files (*.*)\0*.*\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrDefExt = "glb";
				ofn.lpstrTitle = "Load another object";
				ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

				if (GetOpenFileNameA(&ofn)) {
					bige::gltf_loader loader;
					auto obj = loader.load(szFile);
					std::unique_ptr<biwe::scene_object> scene_obj = std::make_unique<biwe::scene_object>();
					scene_obj->attach_model(std::move(obj));
					s->m_objects.push_back(std::move(scene_obj));
				}
			}
		}


		static bool apply_individual_materials = true;
		// Combo box to select a shader to apply to all meshes in the scene
		{
			static int selected_index = 0;
			static const char* options[] = {
				"Manage individual shaders",
				BIGE_NAMEOF_C(bige::materials::pbr_material),
				BIGE_NAMEOF_C(bige::materials::lit_material_normals),
				BIGE_NAMEOF_C(bige::materials::lit_material),
				BIGE_NAMEOF_C(bige::materials::textured_material)
			};
			auto selected = ImGui::Combo("Apply shader to all...", &selected_index, options, IM_ARRAYSIZE(options));
			if (selected) {
				if (selected_index > 0) {
					for (const auto& obj : s->objects_ref()) {
						for (const auto& prim : obj->m_model->primitives_ref()) {
							prim->change_material(options[selected_index]);
						}
					}
				}
			}
			apply_individual_materials = selected_index <= 0;
		}


		int j = 0, i = 0, k = 0;
		if (apply_individual_materials && ImGui::TreeNode("Scene")) {
			for (const auto& obj : s->objects_ref()) {
				k++;
				if (ImGui::TreeNode(("Object: " + std::to_string(k)).c_str())) {
					for (const auto& prim : obj->m_model->primitives_ref()) {

						std::string mesh_info = "Mesh: " + prim->mesh->name + "(" + std::to_string(j++) + ")";
						if (ImGui::TreeNode(mesh_info.c_str())) {
							static const char* options[] = {
									BIGE_NAMEOF_C(bige::materials::pbr_material),
									BIGE_NAMEOF_C(bige::materials::lit_material_normals),
									BIGE_NAMEOF_C(bige::materials::lit_material),
									BIGE_NAMEOF_C(bige::materials::textured_material)
							};
							int selected_index = 0;
							
							if(prim->material->name == BIGE_NAMEOF_C(bige::materials::pbr_material)) {
								selected_index = 0;
							} else if(prim->material->name == BIGE_NAMEOF_C(bige::materials::lit_material_normals)) {
								selected_index = 1;
							} else if(prim->material->name == BIGE_NAMEOF_C(bige::materials::lit_material)) {
								selected_index = 2;
							} else if(prim->material->name == BIGE_NAMEOF_C(bige::materials::textured_material)) {
								selected_index = 3;
							}

							auto selected = ImGui::Combo(mesh_info.c_str(), &selected_index, options, IM_ARRAYSIZE(options));
							if(selected) {
								switch (selected_index) {
									case 0:
										prim->change_material(BIGE_NAMEOF_C(bige::materials::pbr_material));
										break;
									case 1:
										prim->change_material(BIGE_NAMEOF_C(bige::materials::lit_material_normals));
										break;
									case 2:
										prim->change_material(BIGE_NAMEOF_C(bige::materials::lit_material));	
										break;
									case 3:
										prim->change_material(BIGE_NAMEOF_C(bige::materials::textured_material));
										break;
								}
							}
							ImGui::TreePop();
						}
					}
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
		ImGui::InputText(":", gltf_file, sizeof(gltf_file));
		ImGui::SameLine();
		if (ImGui::Button("... (GLB Path)")) {
			char szFile[MAX_PATH] = "";
			OPENFILENAME ofn = {};
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = nullptr;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFilter = "Scene Files (*.glb)\0*.glb\0All Files (*.*)\0*.*\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrDefExt = "glb";
			ofn.lpstrTitle = "Save Scene";
			ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

			if (GetOpenFileNameA(&ofn)) {
				strcpy(gltf_file, szFile);
			}
		}

		ImGui::InputText("##Save As:", save_as, sizeof(save_as));
		ImGui::SameLine();
		if (ImGui::Button("...")) {
#ifdef WIN32
			// Open a file dialog to select a file path
			char szFile[MAX_PATH] = "";
			OPENFILENAME ofn = {};
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = nullptr;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFilter = "Scene Files (*.bisc)\0*.bisc\0All Files (*.*)\0*.*\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrDefExt = "bisc";
			ofn.lpstrTitle = "Save Scene";
			ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

			if (GetSaveFileName(&ofn)) {
				strcpy(save_as, szFile);
			}
#endif
		}
		if (ImGui::Button("Save Object")) {
			
			std::ifstream file_gltf(gltf_file);
			if (!file_gltf.is_open()) return;

			file_gltf.seekg(0, std::ios::end);
			int size = file_gltf.tellg();
			file_gltf.seekg(0, std::ios::beg);
			std::vector<uint8_t> buffer(size);
			file_gltf.read((char*)buffer.data(), size);
			file_gltf.close();
			
			nlohmann::json j;
			j["far_plane"] = bige::orbit_camera::instance->far_plane;
			j["near_plane"] = bige::orbit_camera::instance->near_plane;
			j["azimuth"] = bige::scene::m_sun->azimuth;
			j["elevation"] = bige::scene::m_sun->elevation;
			j["ambient_strength"] = bige::scene::m_sun->ambient_strength;
			j["camera_position"] = { bige::orbit_camera::instance->get_position().x, bige::orbit_camera::instance->get_position().y, bige::orbit_camera::instance->get_position().z }; 
			j["object_type"] = "gltf";
			j["object"] = nlohmann::json::binary_t(buffer);
			std::ofstream file(save_as);
			file << j << std::endl;
			file.close();
		}
		ImGui::End();
	}

}