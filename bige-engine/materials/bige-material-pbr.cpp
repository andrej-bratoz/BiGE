
#include <materials/bige-materials.h>
#include <bige-engine.h>
#include <bige-shadercache.h>
#include <bige-scene.h>
#include <glm/gtc/matrix_inverse.hpp>

namespace bige { namespace materials {
	pbr_material::pbr_material() {
		attach_shader(bige::shader_cache::instance()->get(BIGE_SHADER_VERTEX_PNU_LIT));
		attach_shader(bige::shader_cache::instance()->get(FRAGMENT_TEXTURE_PBR));

		register_uniform({
			"uModel", "uView", "uProjection", "uNormalMatrix",
			"uLightDirection", "uLightColor", "uAmbientColor", "uAmbientStrength",
			"uViewPos", "uAlphaCutoff",
			"uMetallicFactor", "uRoughnessFactor"
		});

		push_uniform("uBaseColorSampler", 0);
		push_uniform("uNormalSampler", 1);
		push_uniform("uMetallicRoughnessSampler", 2);
		name = BIGE_NAMEOF(bige::materials::pbr_material);
	}

	void pbr_material::update_uniform(bige::transform& t) {
		if (base_color_texture) {
			base_color_texture->bind(0);
		}
		if (normal_texture) {
			normal_texture->bind(1);
		}
		if (metallic_roughness_texture) {
			metallic_roughness_texture->bind(2);
		}

		auto view = bige::orbit_camera::instance->get_view_matrix();
		glm::mat4 projection = glm::perspective(glm::radians(bige::orbit_camera::instance->fov),
			(float)bige::Window::engine_window->m_width / (float)bige::Window::engine_window->m_height,
			bige::orbit_camera::instance->near_plane, bige::orbit_camera::instance->far_plane);

		// Assumes push_uniform(name, const glm::mat3&) now exists on material,
		// matching what your vertex_pnu_lit.vs already expects (uniform mat3).
		glm::mat3 normal_matrix = glm::inverseTranspose(glm::mat3(t.world_matrix));

		push_uniform("uModel", t.world_matrix);
		push_uniform("uView", view);
		push_uniform("uProjection", projection);
		push_uniform("uNormalMatrix", normal_matrix);

		push_uniform("uLightDirection", bige::scene::m_sun->direction);
		push_uniform("uLightColor", bige::scene::m_sun->light_color);
		push_uniform("uAmbientColor", bige::scene::m_sun->ambient_color);
		push_uniform("uAmbientStrength", bige::scene::m_sun->ambient_strength);

		push_uniform("uViewPos", bige::orbit_camera::instance->get_position());

		push_uniform("uAlphaCutoff", alpha_cutoff);
		push_uniform("uMetallicFactor", metallic_factor);
		push_uniform("uRoughnessFactor", roughness_factor);
	}
}}