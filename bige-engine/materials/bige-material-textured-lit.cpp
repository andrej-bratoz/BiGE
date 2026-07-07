#include <bige-engine.h>
#include <bige-shadercache.h>
#include <bige-material.h>
#include <materials/bige-materials.h>
#include <glm/gtc/matrix_inverse.hpp>

namespace bige {
	namespace materials {
		lit_material::lit_material() {
			attach_shader(bige::shader_cache::instance()->get(BIGE_SHADER_VERTEX_PNU_LIT));
			attach_shader(bige::shader_cache::instance()->get(FRAGMENT_TEXTURE_LIT));

			register_uniform({
				"uModel", "uView", "uProjection", "uNormalMatrix",
				"uLightDirection", "uLightColor", "uViewPos",
				"uAmbientStrength", "uSpecularStrength", "uShininess", "uAlphaCutoff", "uBaseColorSampler"
			});

			push_uniform("uBaseColorSampler", 0);
			name = BIGE_NAMEOF(bige::materials::lit_material);
		}

		void lit_material::update_uniform(bige::transform& t) {
			if (base_color_texture) {
				base_color_texture->bind(0);
			}

			auto view = bige::orbit_camera::instance->get_view_matrix();
			glm::mat4 projection = glm::perspective(glm::radians(bige::orbit_camera::instance->fov),
				(float)bige::Window::engine_window->m_width / (float)bige::Window::engine_window->m_height,
				bige::orbit_camera::instance->near_plane, bige::orbit_camera::instance->far_plane);

			glm::mat3 normal_mat3 = glm::inverseTranspose(glm::mat3(t.world_matrix));

			push_uniform("uModel", t.world_matrix);
			push_uniform("uView", view);
			push_uniform("uProjection", projection);
			push_uniform("uNormalMatrix", normal_mat3);

			push_uniform("uLightDirection", bige::scene::m_sun->direction);
			push_uniform("uLightColor", bige::scene::m_sun->light_color);
			// TODO: confirm this matches your camera class -- it needs a position
			// accessor parallel to get_view_matrix(). Rename if yours differs
			// (e.g. m_camera->position if it's a public member instead of a getter).
			push_uniform("uViewPos", orbit_camera::instance->get_position());

			push_uniform("uAmbientStrength", bige::scene::m_sun->ambient_strength);
			push_uniform("uSpecularStrength", specular_strength);
			push_uniform("uShininess", shininess);
			push_uniform("uAlphaCutoff", alpha_cutoff);
		}

		lit_material_normals::lit_material_normals() {
			attach_shader(bige::shader_cache::instance()->get(BIGE_SHADER_VERTEX_PNU_LIT_NORMALS));
			attach_shader(bige::shader_cache::instance()->get(FRAGMENT_TEXTURE_LIT_NORMALS));

			register_uniform({
				"uModel", "uView", "uProjection", "uNormalMatrix",
				"uLightDirection", "uLightColor", "uViewPos",
				"uAmbientStrength", "uSpecularStrength", "uShininess", "uAlphaCutoff", "uNormalSampler", "uBaseColorSampler"
			});

			push_uniform("uBaseColorSampler", 0);
		}

		void lit_material_normals::update_uniform(bige::transform& t) {
			push_uniform("uNormalSampler", 1);
			if (base_color_texture) {
				base_color_texture->bind(0);
			}

			auto view = orbit_camera::instance->get_view_matrix();
			glm::mat4 projection = glm::perspective(glm::radians(orbit_camera::instance->fov),
				(float)bige::Window::engine_window->m_width / (float)bige::Window::engine_window->m_height,
				orbit_camera::instance->near_plane, orbit_camera::instance->far_plane);

			glm::mat3 normal_mat3 = glm::inverseTranspose(glm::mat3(t.world_matrix));

			push_uniform("uModel", t.world_matrix);
			push_uniform("uView", view);
			push_uniform("uProjection", projection);
			push_uniform("uNormalMatrix", normal_mat3);
			push_uniform("uLightDirection", bige::scene::m_sun->direction);
			push_uniform("uLightColor", bige::scene::m_sun->light_color);
			// TODO: confirm this matches your camera class -- it needs a position
			// accessor parallel to get_view_matrix(). Rename if yours differs
			// (e.g. m_camera->position if it's a public member instead of a getter).
			push_uniform("uViewPos", bige::orbit_camera::instance->get_position());

			push_uniform("uAmbientStrength", bige::scene::m_sun->ambient_strength);
			push_uniform("uSpecularStrength", specular_strength);
			push_uniform("uShininess", shininess);
			push_uniform("uAlphaCutoff", alpha_cutoff);
		}
	}
}