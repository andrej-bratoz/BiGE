#include <bige-mesh.h>
#include <bige-utils.h>
#include <algorithm>

namespace bige {
	void mesh::add_vertex(const vertex& v) { m_vertices.emplace_back(v); }
	void mesh::add_index(uint32_t index) { m_indices.push_back(index); }
	mesh::vertex* mesh::vertices() { return m_vertices.data(); }
	uint32_t* mesh::indices() { return m_indices.data(); }

	//
	int mesh::vertices_count() const { return static_cast<int>(m_vertices.size()); }
	int mesh::indices_count() const { return static_cast<int>(m_indices.size()); }

	//
	void mesh::caulcuate_bounds() {
		if (m_vertices.empty()) { return; }
		m_bounds.minimal_x = m_bounds.maximal_x = static_cast<int>(m_vertices[0].position.x);
		m_bounds.minimal_y = m_bounds.maximal_y = static_cast<int>(m_vertices[0].position.y);
		m_bounds.minimal_z = m_bounds.maximal_z = static_cast<int>(m_vertices[0].position.z);
		for (const auto& v : m_vertices) {
			if (v.position.x < m_bounds.minimal_x) { m_bounds.minimal_x = static_cast<int>(v.position.x); }
			if (v.position.x > m_bounds.maximal_x) { m_bounds.maximal_x = static_cast<int>(v.position.x); }
			if (v.position.y < m_bounds.minimal_y) { m_bounds.minimal_y = static_cast<int>(v.position.y); }
			if (v.position.y > m_bounds.maximal_y) { m_bounds.maximal_y = static_cast<int>(v.position.y); }
			if (v.position.z < m_bounds.minimal_z) { m_bounds.minimal_z = static_cast<int>(v.position.z); }
			if (v.position.z > m_bounds.maximal_z) { m_bounds.maximal_z = static_cast<int>(v.position.z); }
		}
	}
}