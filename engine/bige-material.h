
#pragma once
#include <bige-texture.h>
#include <bige-shader.h>
#include <bige-transform.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <map>
#include <functional>



namespace bige {

	struct material {

		struct uniform_val {
			enum class type {
				i32,
				f32,
				vec2,
				vec3,
				vec4,
				mat3,
				mat4
			};

			int32_t location = -1;
			glm::vec4 value;
			glm::mat3 value_mat3;
			glm::mat4 value_mat4;
			int32_t value_int = 0;
			type actual_type = type::vec4;
		};

		glm::vec4 color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f); // default gray

		std::shared_ptr<texture> base_color_texture = nullptr;
		std::shared_ptr<texture> normal_texture = nullptr;
		std::shared_ptr<texture> metallic_roughness_texture = nullptr;

		std::vector<std::shared_ptr<bige::shader>> shaders;
		std::shared_ptr<bige::shader_program> program = nullptr;
		std::map<std::string, uniform_val> uniform_locations;

		void register_uniform(const std::string& name);
		void register_uniform(const std::initializer_list<std::string>& names);
		void attach_shader(std::shared_ptr<bige::shader> shader);
		

		void push_uniform(const std::string& name, int32_t value);
		void push_uniform(const std::string& name, float value);
		void push_uniform(const std::string& name, const glm::vec2& value);
		void push_uniform(const std::string& name, const glm::vec3& value);
		void push_uniform(const std::string& name, const glm::vec4& value);
		void push_uniform(const std::string& name, const glm::mat3& value);
		void push_uniform(const std::string& name, const glm::mat4& value);
		void fill_uniform_locations();
		void set_uniforms(bige::transform& t);
		bool compile_shaders();
		virtual void update_uniform(bige::transform& t) = 0;
		virtual ~material() = default;
	private:
		bool m_shader_compiled = false;
	};
}