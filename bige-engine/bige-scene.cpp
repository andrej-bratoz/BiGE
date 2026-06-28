#include <bige-scene.h>
#include <bige-model.h>
#include <bige-object.h>

namespace bige {
	const std::vector<std::unique_ptr<objects::object_3d>>& scene::objects_ref() { return m_objects; }
	void scene::add_object(std::unique_ptr<objects::object_3d> obj) { m_objects.push_back(std::move(obj)); }
	size_t scene::primitives_count() { return m_objects.size(); }
}