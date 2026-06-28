#include <bige-model.h>

namespace bige {
	void bige::model::add_primitive(std::unique_ptr<primitive> p) { m_primives.push_back(std::move(p)); }
	std::vector<std::unique_ptr<model::primitive>> model::primitives() { return std::move(m_primives); }
	const std::vector<std::unique_ptr<model::primitive>>& model::primitives_ref() { return m_primives; }
	size_t model::primitives_count() const { return (size_t)m_primives.size(); }
}
