#pragma once
#include <memory>
#include <vector>
#include <bige-object.h>

namespace bige {

	struct direction_light {
		glm::vec3 direction = glm::normalize(glm::vec3(-0.3f, -1.0f, -0.2f));
		glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 ambient_color = glm::vec3(1.0f, 1.0f, 1.0f);
		float ambient_strength = 0.85f;
	};


	struct scene {
		const std::vector<std::unique_ptr<objects::object_3d>>& objects_ref();
		void add_object(std::unique_ptr<objects::object_3d> obj);
		size_t primitives_count();
		std::vector<std::unique_ptr<objects::object_3d>> m_objects;
		// For now, just a single directional light -- swap in a more robust lighting system when you want to support more lights or different light types.
		inline static std::unique_ptr< direction_light> m_sun = std::make_unique<direction_light>();

	};
}