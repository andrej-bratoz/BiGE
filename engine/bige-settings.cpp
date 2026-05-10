#include <bige-utils.h>
#include <bige-settings.h>
#include <fstream>

namespace bige {
	//
	std::string settings::config_folder() {
		std::filesystem::path path(bige::exe_dir());
		path.append("settings");
		path.make_preferred();
		return path.string();
	}

	std::string settings::shader_folder() {
		std::filesystem::path path(bige::exe_dir());
		path.append("shaders");
		path.make_preferred();
		return path.string();
	}

	std::string settings::assets_folder() {
		std::filesystem::path path(bige::exe_dir());
		path.append("assets");
		path.make_preferred();
		return path.string();
	}

	std::string settings::read_text_file(const std::string& relative_path) {
		std::ifstream file(relative_path);
		if (!file.is_open()) {
			return "";
		}
		return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	}

	std::string settings::read_text_file(const std::string& root, const std::string& file) {
		std::filesystem::path path(root);
		path.append(file);
		std::ifstream infile(path);
		if (!infile.is_open()) {
			return "";
		}
		return std::string((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
	}
}
