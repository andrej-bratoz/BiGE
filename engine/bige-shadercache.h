#pragma once
#include <bige-shader.h>
#include <map>
#include <string>
#include <memory>

namespace bige {

	constexpr const char* VERTEX_OFFSET_3D = "shaders.vertex.offset_3d";
	constexpr const char* VERTEX_TEXTURE_3D = "shaders.vertex.texture_3d";
	constexpr const char* BIGE_SHADER_VERTEX_PNU = "shaders.vertex.pnu";
	constexpr const char* BIGE_SHADER_VERTEX_PNU_LIT = "shaders.vertex.pnu.light";
	constexpr const char* BIGE_SHADER_VERTEX_PNU_LIT_NORMALS = "shaders.vertex.pnu.light.normals";

	constexpr const char* FRAGMENT_SOLID_COLOR_RGBA = "shaders.fragment.solid_color_rgba";
	constexpr const char* FRAGMENT_TEXTURE_BASIC = "shaders.fragment.texture_basic";
	constexpr const char* FRAGMENT_TEXTURE_LIT = "shaders.fragment.texture_3d.light";
	constexpr const char* FRAGMENT_TEXTURE_LIT_NORMALS = "shaders.fragment.texture_3d.light.normals";
	constexpr const char* FRAGMENT_TEXTURE_PBR = "shaders.fragment.texture_3d.pbr";

	class shader_cache {
	public:
		bool add(const std::string& name, const std::string& content, bige::shader::shader_type type);
		std::shared_ptr<bige::shader> get(const std::string& name) const;
		std::string last_error() const;
		static std::shared_ptr<shader_cache> instance() { return s_instance; }

	private:
		std::map<std::string, std::shared_ptr<bige::shader>> m_cache;
		std::string m_last_error;

		inline static std::shared_ptr<shader_cache> s_instance = std::make_shared<shader_cache>();
	};
}