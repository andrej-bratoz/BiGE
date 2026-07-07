#pragma once
#include <bige-material.h>

namespace bige { namespace materials {
	struct solid_color_material : bige::material {
		solid_color_material(glm::vec4 color);
		virtual void update_uniform(bige::transform& t) override;
		glm::vec4 m_color;
	};

	struct textured_material : bige::material {
		textured_material();
		virtual void update_uniform(bige::transform& t) override;
	};

	struct lit_material : bige::material {
		lit_material();
		virtual void update_uniform(bige::transform& t) override;

		// Basic directional ("sun") light. Hardcoded here for now --
		// promote these to scene/engine-level settings once you want
		// more than one light or to tweak it without recompiling.
		glm::vec3 light_direction = glm::normalize(glm::vec3(-0.3f, -1.0f, -0.2f));
		glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f);
		float specular_strength = 0.5f;
		float shininess = 32.0f;
		float alpha_cutoff = 0.5f; // matches glTF's default alphaCutoff for MASK materials
	};

	struct lit_material_normals : bige::material {
		lit_material_normals();
		virtual void update_uniform(bige::transform& t) override;

		// Basic directional ("sun") light. Hardcoded here for now --
		// promote these to scene/engine-level settings once you want
		// more than one light or to tweak it without recompiling.
		float specular_strength = 0.5f;
		float shininess = 32.0f;
		float alpha_cutoff = 0.5f; // matches glTF's default alphaCutoff for MASK materials
	};

	struct pbr_material : bige::material {
		pbr_material();
		void update_uniform(bige::transform& t) override;

		std::shared_ptr<texture> normal_texture = nullptr;
		std::shared_ptr<texture> metallic_roughness_texture = nullptr;

		float alpha_cutoff = 0.5f; // per-material -- ideally set from the glTF's own alphaCutoff (MASK materials)
		float metallic_factor = 1.0f;
		float roughness_factor = 1.0f;
	};


	bige::material* choose_material_tglf(void* mat, std::shared_ptr<bige::texture> tex, std::shared_ptr<bige::texture> normal_tex, std::shared_ptr<bige::texture> rough_met);
}}