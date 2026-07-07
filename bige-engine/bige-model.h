#pragma once
#include <string>
#include <bige-material.h>
#include <bige-shadercache.h>
#include <materials/bige-materials.h>
#include <bige-mesh.h>
#include <glm/vec3.hpp>
#include <glm/mat4x2.hpp>
#include <glm/gtc/quaternion.hpp>

#define BIGE_NAMEOF(x) std::string(#x)
#define BIGE_NAMEOF_C(x) (#x)

namespace bige {

	struct model {
		struct primitive {
			std::unique_ptr<bige::mesh> mesh;
			std::unique_ptr<bige::material> material;
			bige::transform transform;
			void change_material(const std::string& name);
		};

		glm::vec3 position;
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
		glm::vec3 rotation;

		void add_primitive(std::unique_ptr<primitive> p);
		std::vector<std::unique_ptr<primitive>> primitives();
		const std::vector<std::unique_ptr<primitive>>& primitives_ref();
		size_t primitives_count() const;
	private:
		std::vector<std::unique_ptr<primitive>> m_primives;
	};


	struct dynamic_material_switcher {
		template<typename T>
		inline static void switch_material(std::unique_ptr<bige::material>& material) {
			auto new_mat = new T();
			new_mat->base_color_texture = material->base_color_texture;
			new_mat->normal_texture = material->normal_texture;
			new_mat->metallic_roughness_texture = material->metallic_roughness_texture;
			material->program->drop_program();
			material->program->reset();
			new_mat->compile_shaders();
			new_mat->program->use_program();
			new_mat->program->link();
			new_mat->fill_uniform_locations();
			material.reset(new_mat);
		}
	};
}