#pragma once
#include <string>
#include <filesystem>
#ifdef WIN32
#	include <Windows.h>
#endif

namespace bige {

#	define BIGE_SETTINGS_VERTEX_SHADER_FOLDER (std::filesystem::path(bige::settings::shader_folder()).append("vertex").generic_string())
#	define BIGE_SETTINGS_FRAGMENT_SHADER_FOLDER (std::filesystem::path(bige::settings::shader_folder()).append("fragment").generic_string())

	struct settings {
		static std::string config_folder();
		static std::string shader_folder();
		static std::string assets_folder();
		static std::string read_text_file(const std::string& relative_path);
		static std::string read_text_file(const std::string& root, const std::string& file);
	};

}