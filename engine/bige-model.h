#pragma once
#include <bige-material.h>
#include <bige-mesh.h>
#include <glm/vec3.hpp>
#include <glm/mat4x2.hpp>
#include <glm/gtc/quaternion.hpp>

namespace bige {

	struct model {
		struct primitive {
			std::unique_ptr<bige::mesh> mesh;
			std::unique_ptr<bige::material> material;
			bige::transform transform;
		};

		glm::vec3 position;
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
		glm::vec3 rotation;

		void add_primitive(std::unique_ptr<primitive> p);
		std::vector<std::unique_ptr<primitive>> primitives();
		const std::vector<std::unique_ptr<primitive>>& primitives_ref();
		//primitive* operator[](int index) const { return m_primives[index].get(); }
		size_t primitives_count() const;
	private:
		std::vector<std::unique_ptr<primitive>> m_primives;
	};
}