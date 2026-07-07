#include <bige-model.h>

namespace bige {
	void bige::model::add_primitive(std::unique_ptr<primitive> p) { m_primives.push_back(std::move(p)); }
	std::vector<std::unique_ptr<model::primitive>> model::primitives() { 
		return std::move(m_primives); 
	}
	
	const std::vector<std::unique_ptr<model::primitive>>& model::primitives_ref() { 
		return m_primives; 
	}
	
	size_t model::primitives_count() const { 
		return (size_t)m_primives.size(); 
	}
	
	void model::primitive::change_material(const std::string& name) {
		if (name == BIGE_NAMEOF(bige::materials::textured_material)) {
			dynamic_material_switcher::switch_material<bige::materials::textured_material>(material);
		} else if(name == BIGE_NAMEOF(bige::materials::lit_material)) {
			dynamic_material_switcher::switch_material<bige::materials::lit_material>(material);
		} else if(name == BIGE_NAMEOF(bige::materials::lit_material_normals)) {
			dynamic_material_switcher::switch_material<bige::materials::lit_material_normals>(material);
		} else if(name == BIGE_NAMEOF(bige::materials::pbr_material)) {
			dynamic_material_switcher::switch_material<bige::materials::pbr_material>(material);
		}
		material->name = name;
	}
}
