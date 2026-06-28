#include <bige-engine.h>
#include <bige-shadercache.h>
#include <bige-material.h>
#include <materials/bige-materials.h>

namespace bige { namespace materials {
	textured_material::textured_material() {
		attach_shader(bige::shader_cache::instance()->get(BIGE_SHADER_VERTEX_PNU));
		attach_shader(bige::shader_cache::instance()->get(FRAGMENT_TEXTURE_BASIC));

		register_uniform({ "uModel", "uView", "uProjection" });
		push_uniform("uBaseColorSampler", 0);
	}
	void textured_material::update_uniform(bige::transform& t) {
		if (base_color_texture) {
			base_color_texture->bind(0);
		}
		auto view = bige::orbit_camera::instance->get_view_matrix();
		glm::mat4 projection = glm::perspective(glm::radians(bige::orbit_camera::instance->fov), 
			(float)bige::Window::width / (float)bige::Window::height, 
			bige::orbit_camera::instance->near_plane, bige::orbit_camera::instance->far_plane);

		push_uniform("uModel", t.world_matrix);
		push_uniform("uView", view);
		push_uniform("uProjection", projection);
	}
}}