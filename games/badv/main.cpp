#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <bige-engine.h>
#include <bige-utils.h>
#include <bige-state_cb.h>
#include <bige-settings.h>
#include <bige-logging.h>
#include <bige-object.h>
#include <bige-shadercache.h>
#include <Windows.h>
#include <memory>
//
#include "ba-scenes.h"

void init_materials() {
	bige::shader_cache::instance()->add(bige::VERTEX_OFFSET_3D, bige::settings::read_text_file(BIGE_SETTINGS_VERTEX_SHADER_FOLDER, "offset_3d.vs"), bige::shader::shader_type::vertex);
	bige::shader_cache::instance()->add(bige::VERTEX_TEXTURE_3D, bige::settings::read_text_file(BIGE_SETTINGS_VERTEX_SHADER_FOLDER, "texture_3d.vs"), bige::shader::shader_type::vertex);
	bige::shader_cache::instance()->add(bige::BIGE_SHADER_VERTEX_PNU, bige::settings::read_text_file(BIGE_SETTINGS_VERTEX_SHADER_FOLDER, "vertex_pnu.vs"), bige::shader::shader_type::vertex);
	bige::shader_cache::instance()->add(bige::BIGE_SHADER_VERTEX_PNU_LIT, bige::settings::read_text_file(BIGE_SETTINGS_VERTEX_SHADER_FOLDER, "vertex_pnu_lit.vs"), bige::shader::shader_type::vertex);
	bige::shader_cache::instance()->add(bige::BIGE_SHADER_VERTEX_PNU_LIT_NORMALS, bige::settings::read_text_file(BIGE_SETTINGS_VERTEX_SHADER_FOLDER, "vertex_pnu_lit_normals.vs"), bige::shader::shader_type::vertex);

	bige::shader_cache::instance()->add(bige::FRAGMENT_SOLID_COLOR_RGBA, bige::settings::read_text_file(BIGE_SETTINGS_FRAGMENT_SHADER_FOLDER, "solid_color_rgba.fs"), bige::shader::shader_type::fragment);
	bige::shader_cache::instance()->add(bige::FRAGMENT_TEXTURE_BASIC, bige::settings::read_text_file(BIGE_SETTINGS_FRAGMENT_SHADER_FOLDER, "texture.fs"), bige::shader::shader_type::fragment);
	bige::shader_cache::instance()->add(bige::FRAGMENT_TEXTURE_LIT, bige::settings::read_text_file(BIGE_SETTINGS_FRAGMENT_SHADER_FOLDER, "texture_lit.fs"), bige::shader::shader_type::fragment);
	bige::shader_cache::instance()->add(bige::FRAGMENT_TEXTURE_LIT_NORMALS, bige::settings::read_text_file(BIGE_SETTINGS_FRAGMENT_SHADER_FOLDER, "texture_lit_normal.fs"), bige::shader::shader_type::fragment);
	bige::shader_cache::instance()->add(bige::FRAGMENT_TEXTURE_PBR, bige::settings::read_text_file(BIGE_SETTINGS_FRAGMENT_SHADER_FOLDER, "texture_pbr.fs"), bige::shader::shader_type::fragment);
}


int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	_log("main");
	_l("=================== starting BiGE engine... ===================");
	std::unique_ptr<bige::engine> engine = std::make_unique<bige::engine>();
	if (!engine->initialize()) {
		_l("engine failed to initialize");
		MessageBoxA(NULL, "Failed to create window", "Error", MB_OK | MB_ICONERROR);
		return -1;
	}
	_l("engine initialized successfully");
	
	BIGE_DEFER(
		_log("main");
		_l("engine loop has ended");
		engine->shutdown();
		_l("engine shutdown called");
		bige::logger::s_logger->close();
	);
	
	bige::engine_state st;
	st.state_set(bige::state::WINDOW_BACKGROUND_COLOR, BIGE_STATE_VEC4(30, 40, 40, 255), true);
	
	_l("init materials");
	init_materials();
	
	_l("creating scene");
	std::shared_ptr<bige::scene> scene = std::make_shared<ba::default_scene>();
	engine->set_scene(scene);
	
	_l("entering engine loop");
	engine->run();
	
	return 0;
}