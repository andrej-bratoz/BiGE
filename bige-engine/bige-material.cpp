#include <bige-material.h>
#include <bige-engine.h>
#include <bige-state_cb.h>
#include <bige-utils.h>
#include <bige-logging.h>
#include <bige-transform.h>
#include <glm/gtc/type_ptr.hpp>

namespace bige {
	void material::register_uniform(const std::string& name) {
		uniform_locations[name] = uniform_val();
	}
	void material::register_uniform(const std::initializer_list<std::string>& names) {
		for (const auto& name : names) {
			register_uniform(name);
		}
	}
	void material::attach_shader(std::shared_ptr<bige::shader> shader) {
		shaders.push_back(shader);
	}

	void material::push_uniform(const std::string& name, int32_t value) {
		if (uniform_locations.count(name) == 0) {
			return;
		}
		uniform_locations[name].actual_type = material::uniform_val::type::i32;
		uniform_locations[name].value_int = value;
	}

	void material::push_uniform(const std::string& name, float value) {
		if (uniform_locations.count(name) == 0) {
			return;
		}
		uniform_locations[name].actual_type = material::uniform_val::type::f32;
		uniform_locations[name].value = glm::vec4(value, 0.0f, 0.0f, 0.0f);
	}
	void material::push_uniform(const std::string& name, const glm::vec2& value) {
		if (uniform_locations.count(name) == 0) {
			return;
		}
		uniform_locations[name].actual_type = material::uniform_val::type::vec2;
		uniform_locations[name].value = glm::vec4(value, 0.0f, 0.0f);
	}
	void material::push_uniform(const std::string& name, const glm::vec3& value) {
		if (uniform_locations.count(name) == 0) {
			return;
		}
		uniform_locations[name].actual_type = material::uniform_val::type::vec3;
		uniform_locations[name].value = glm::vec4(value, 0.0f);
	}
	void material::push_uniform(const std::string& name, const glm::vec4& value) {
		if (uniform_locations.count(name) == 0) {
			return;
		}
		uniform_locations[name].actual_type = material::uniform_val::type::vec4;
		uniform_locations[name].value = value;
	}

	void material::push_uniform(const std::string& name, const glm::mat3& value) {
		if (uniform_locations.count(name) == 0) {
			return;
		}
		uniform_locations[name].actual_type = material::uniform_val::type::mat3;
		uniform_locations[name].value_mat3 = value;
	}

	void material::push_uniform(const std::string& name, const glm::mat4& value) {
		if (uniform_locations.count(name) == 0) {
			return;
		}
		uniform_locations[name].actual_type = material::uniform_val::type::mat4;
		uniform_locations[name].value_mat4 = value;
	}

	void material::fill_uniform_locations() {
		if (program) {
			program->use_program();
			for (auto& [name, loc] : uniform_locations) {
				loc.location = glGetUniformLocation(program->id(), name.c_str());
			}
			program->drop_program();
		}
	}

	void material::set_uniforms(bige::transform& t) {
		update_uniform(t);
		_log("material");
		glUseProgram(program->id());
		for (auto& [name, val] : uniform_locations) {
			switch (val.actual_type) {
				case material::uniform_val::type::i32:
					glUniform1i(val.location, val.value_int);
					break;
				case material::uniform_val::type::f32:
					glUniform1f(val.location, val.value.x);
					break;
				case material::uniform_val::type::vec2:
					glUniform2f(val.location, val.value.x, val.value.y);
					break;
				case material::uniform_val::type::vec3:
					glUniform3f(val.location, val.value.x, val.value.y, val.value.z);
					break;
				case material::uniform_val::type::vec4:
					glUniform4f(val.location, val.value.x, val.value.y, val.value.z, val.value.w);
					break;
				case material::uniform_val::type::mat3:
					glUniformMatrix3fv(val.location, 1, GL_FALSE, glm::value_ptr(val.value_mat3));
					break;
				case material::uniform_val::type::mat4:
					glUniformMatrix4fv(val.location, 1, GL_FALSE, glm::value_ptr(val.value_mat4));
					break;
				default: 
					_l("unexpected uniform type, expected value between 0 and 4, got {}", (int)val.actual_type);
					break;
			}
		}
	}
	bool material::compile_shaders() {
		if (m_shader_compiled) return true;
		program = std::make_shared<bige::shader_program>();
		for (auto& sh : shaders) {
			if (!sh->compile()) {
				_log("material");
				_l("shader compilation failed with error: {}", sh->error());
				program->reset();
				return false;
			}
			program->attach_shader(sh);
		}
		if (!program->link()) {
			_log("material");
			_l("linking shader program failed with error {}", program->error());
			program->reset();
			return false;
		}
		m_shader_compiled = true;
		return true;
	}
}