#pragma once
#include <string>
#include <vector>
#include <fstream>

namespace bige {
	struct shader {
		enum class shader_type {
			vertex,
			fragment,
			geometry
		};

		shader() = default;
		void from_string(const std::string& program);
		void from_file(const std::string& path);
		const std::string& error() const { return m_error; }
		virtual bool compile() = 0;
		virtual shader_type type() const = 0;
		virtual unsigned int id() const = 0;

	protected:
		std::string m_program;
		std::string m_error;
	};

	struct vertex_shader : public shader {
		virtual bool compile() override;
		virtual shader_type type() const override { return shader_type::vertex; }
		virtual unsigned int id() const override { return m_vertex_shader_id; }
		static std::shared_ptr<vertex_shader> create_common();
	private:
		unsigned int m_vertex_shader_id;
	};

	struct fragment_shader : public shader {
		virtual bool compile() override;
		virtual shader_type type() const override { return shader_type::fragment; }
		virtual unsigned int id() const override { return m_fragment_shader_id; }
		static std::shared_ptr<fragment_shader> create_common();
	private:
		unsigned int m_fragment_shader_id;
	};

	struct shader_program {
		shader_program() : m_attached_shaders({}), m_program_id(0) {}
		void attach_shader(std::shared_ptr<shader> shader);
		void reset();
		void use_program() const;
		void drop_program() const;
		bool link();
		unsigned int id() const;
		const std::string& error();

	private:
		std::vector<std::shared_ptr<shader>> m_attached_shaders;
		unsigned int m_program_id;
		std::string m_error;
	};
}
