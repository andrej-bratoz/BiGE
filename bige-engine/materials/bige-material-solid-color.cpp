#include <bige-engine.h>
#include <bige-shadercache.h>
#include <bige-material.h>
#include <materials/bige-materials.h>

namespace bige { namespace materials {
	solid_color_material::solid_color_material(glm::vec4 color) : m_color(color) {
		attach_shader(bige::shader_cache::instance()->get(VERTEX_OFFSET_3D));
		attach_shader(bige::shader_cache::instance()->get(FRAGMENT_SOLID_COLOR_RGBA));
		register_uniform({ "uColor", "uOffset"});
		name = BIGE_NAMEOF(bige::materials::solid_color_material);
	}

	void solid_color_material::update_uniform(bige::transform& t) {
		float time = (float)glfwGetTime();
		push_uniform("uColor", m_color);
	}
}}