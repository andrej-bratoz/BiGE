#pragma once
#include <memory>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/glm.hpp>

namespace bige {
	struct mesh {

		struct mesh_bounds {
			int minimal_x;
			int maximal_x;
			int minimal_y;
			int maximal_y;
			int minimal_z;
			int maximal_z;
		};

		struct vertex {
			vertex(const glm::vec3& pos, const glm::vec3& normal, const glm::vec2& uv) : 
				position(pos), normal(normal), uv(uv), tangent(glm::vec4(0.0f)) {
				glm::vec3 helper = (std::abs(normal.y) < 0.99f) ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
				glm::vec3 tang = glm::normalize(glm::cross(helper, normal));
				tangent = glm::vec4(tang, 1.0f);
			}

			vertex(const glm::vec3& pos, const glm::vec3& normal, const glm::vec2& uv, glm::vec4 tangent) :
				position(pos), normal(normal), uv(uv), tangent(tangent) {
			}

			glm::vec4 tangent;
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 uv;
			static consteval size_t size() { return sizeof(vertex); }
		};

		void add_vertex(const vertex& v);
		void add_index(uint32_t index);
		//
		vertex* vertices();
		uint32_t* indices();
		//
		int vertices_count() const;
		int indices_count() const;
		//
		void caulcuate_bounds();

		static std::unique_ptr<bige::mesh> create_triangle_mesh() {
			std::unique_ptr<bige::mesh> m = std::unique_ptr<bige::mesh>(new bige::mesh());
			m->add_vertex(vertex{ glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)	});
			m->add_vertex(vertex{ glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)	});
			m->add_vertex(vertex{ glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)	});
			m->caulcuate_bounds();
			return m;
		}

	private: 
		std::vector<vertex> m_vertices;
		std::vector<uint32_t>  m_indices;
		mesh_bounds m_bounds;
	};
}