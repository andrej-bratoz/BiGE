#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x2.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace bige {
	struct transform {

		// Why quaternion for rotation instead of Euler angles? (pitch, roll yaw) - interesting trivia
		//
		// Euler angles suffer from gimbal lock — when two rotation axes align,
		// you lose a degree of freedom and certain orientations become unreachable.
		// This is not a bug but a mathematical inevitability:
		//
		// 3D rotations form a space called SO(3), which topology proves cannot be
		// mapped smoothly onto R^3 (3 parameters) without singularities.
		//
		// Quaternions use 4 parameters and live in S^3 (a 4D sphere), which is a
		// double cover of SO(3) — meaning every rotation maps cleanly without
		// singularities. The only redundancy is that q and -q represent the same
		// rotation, which is harmless in practice.
		//
		// GLM handles normalization automatically, so we just use glm::quat directly.


		transform() = default;
		transform(const glm::mat4& matrix) : world_matrix(matrix) {}

		glm::mat4 world_matrix = glm::mat4(1.0f);
		glm::vec3 translation() const {
			return glm::vec3(world_matrix[3]);
		}

		glm::quat rotation() const {
			return glm::quat_cast(world_matrix);
		}

		glm::vec3 scale() const {
			float scale_x = glm::length(glm::vec3(world_matrix[0]));
			float scale_y = glm::length(glm::vec3(world_matrix[1]));
			float scale_z = glm::length(glm::vec3(world_matrix[2]));
			return glm::vec3(scale_x, scale_y, scale_z);
		}

		void set_rotation(const glm::quat& q) {
			glm::mat4 rot = glm::toMat4(q);
			glm::vec3 t = translation();
			glm::vec3 s = scale();
			world_matrix = glm::scale(rot, s);
			world_matrix[3] = glm::vec4(t, 1.0f);
		}

		void set_scale(const glm::vec3& s) {
			glm::quat r = rotation();
			glm::vec3 t = translation();
			world_matrix = glm::scale(glm::toMat4(r), s);
			world_matrix[3] = glm::vec4(t, 1.0f);
		}
	};
}