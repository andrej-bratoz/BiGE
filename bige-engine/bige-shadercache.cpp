#include <fstream>
#include <bige-shadercache.h>
#include <bige-logging.h>

namespace bige {
	bool shader_cache::add(const std::string& name, const std::string& content, bige::shader::shader_type type) {
		_log("shader cache");
		auto compile_shader = [this](std::shared_ptr<bige::shader> shader, std::string_view content, std::string_view name) {
			_log("shader compilation");
			_l("compiling shader '{}' to cache", name);
			shader->from_string(content.data());
			if (shader->compile()) {
				m_cache[name.data()] = shader;
				_l("shader '{}' compiled and added to cache successfully", name);
				return true;
			}
			else {
				m_last_error = shader->error();
				_l("failed to compile shader '{}', error: {}", name, m_last_error);
				return false;
			}
		};
		switch (type)	
		{
		case bige::shader::shader_type::vertex:
		{
			std::shared_ptr<vertex_shader> shader = std::shared_ptr<vertex_shader>(new vertex_shader());
			_l("compiling vertex shader '{}'", name);
			return compile_shader(shader, content, name);
		}
		case bige::shader::shader_type::fragment:
		{
			std::shared_ptr<fragment_shader> shader = std::shared_ptr<fragment_shader>(new fragment_shader());
			_l("compiling fragment shader '{}'", name);
			return compile_shader(shader, content, name);
		}
		case bige::shader::shader_type::geometry:
			_l("geometry shader type specified for shader '{}', but geometry shader is not supported yet", name);
			break;
		default:
			_l("invalid shader type specified for shader '{}'", name);
			break;
		}
		return false;
	}
	std::shared_ptr<bige::shader> bige::shader_cache::get(const std::string& name) const {
		if (m_cache.count(name)) return m_cache.at(name);
		return nullptr;
	}
	std::string shader_cache::last_error() const { return m_last_error; }
}