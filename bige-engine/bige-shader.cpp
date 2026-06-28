#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <bige-shader.h>
#include <bige-engine.h>
#include <bige-utils.h>
#include <bige-state_cb.h>

void bige::shader::from_string(const std::string& program) {
	m_program = program;
}

void bige::shader::from_file(const std::string& path) {

}

bool bige::vertex_shader::compile() {
	m_vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
	const char* program_cstr = m_program.c_str();
	glShaderSource(m_vertex_shader_id, 1, &program_cstr, NULL);
	glCompileShader(m_vertex_shader_id);
	int success;
	glGetShaderiv(m_vertex_shader_id, GL_COMPILE_STATUS, &success);
	if(!success) {
		char info_log[1024];
		glGetShaderInfoLog(m_vertex_shader_id, sizeof(info_log), NULL, info_log);
		m_error = std::string("Vertex shader compilation error. ") + info_log;
		return false;
	}
	return true;
}

bool bige::fragment_shader::compile() {
	m_fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
	const char* program_cstr = m_program.c_str();
	glShaderSource(m_fragment_shader_id, 1, &program_cstr, NULL);
	glCompileShader(m_fragment_shader_id);
	int success;
	glGetShaderiv(m_fragment_shader_id, GL_COMPILE_STATUS, &success);
	if(!success) {
		char info_log[1024];
		glGetShaderInfoLog(m_fragment_shader_id, sizeof(info_log), NULL, info_log);
		m_error = std::string("Fragment shader compilation error. ") + info_log;
		return false;
	}
	return true;
}

std::shared_ptr<bige::vertex_shader>  bige::vertex_shader::create_common() {
	std::shared_ptr<vertex_shader> shader = std::make_shared<vertex_shader>();
	std::string common_vertex_shader_program ="#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"uniform vec3 uOffset;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x + uOffset.x, aPos.y + uOffset.y, aPos.z + uOffset.z, 1.0);\n"
		"}\0";
	shader->from_string(common_vertex_shader_program);
	if(!shader->compile()) return nullptr;
	return shader;
}

std::shared_ptr<bige::fragment_shader> bige::fragment_shader::create_common() {
	std::shared_ptr<fragment_shader> shader = std::make_shared<fragment_shader>();
	std::string commoin_vertex_shader_program ="#version 330 core\n"
		"out vec4 FragColor;\n"
		"uniform vec4 uColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = uColor;\n"
		"}\n\0";
	shader->from_string(commoin_vertex_shader_program);
	if (!shader->compile()) return nullptr;
	return shader;
}

void bige::shader_program::attach_shader(std::shared_ptr<shader> shader) {
	if (!shader) return;
	m_attached_shaders.emplace_back(shader);
}

void bige::shader_program::reset() {
	m_attached_shaders.clear();
	if (m_program_id) {
		glDeleteProgram(m_program_id);
		m_program_id = 0;
	}
}

bool bige::shader_program::link() {
	m_program_id = glCreateProgram();

	for (auto& shader : m_attached_shaders) {
		if (!shader->compile()) { return false; }   // assume idempotent
		glAttachShader(m_program_id, shader->id());
	}

	glLinkProgram(m_program_id);

	GLint success;
	glGetProgramiv(m_program_id, GL_LINK_STATUS, &success);
	if (!success) {
		char info_log[1024];
		glGetProgramInfoLog(m_program_id, sizeof(info_log), NULL, info_log);
		m_error = info_log;
	}

	// detach (not delete) — frees the link-time association,
	// but shader objects remain valid for use in other programs
	for (auto& shader : m_attached_shaders) {
		glDetachShader(m_program_id, shader->id());
	}

	return success;
}

unsigned int bige::shader_program::id() const { return m_program_id; }
const std::string& bige::shader_program::error() { return m_error; }
void bige::shader_program::use_program() const { glUseProgram(m_program_id); }
void bige::shader_program::drop_program() const { glUseProgram(0); }


