#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

namespace bige {
	constexpr float ORBIT_YAW = -90.0f; // degrees, starting angle around Y axis
	constexpr float ORBIT_PITCH = 20.0f; // degrees, starting tilt
	constexpr float ORBIT_DISTANCE = 5.0f;
	constexpr float ORBIT_MIN_DIST = 1.0f;
	constexpr float ORBIT_MAX_DIST = 50.0f;
	constexpr float ORBIT_SENSITIVITY = 0.25f; // degrees per pixel of drag
	constexpr float ORBIT_ZOOM_SPEED = 0.5f;  // distance units per scroll tick
	constexpr float ORBIT_FOV = 45.0f;
	constexpr float ORBIT_MIN_PITCH = -89.0f;
	constexpr float ORBIT_MAX_PITCH = 89.0f;
	constexpr float ORBIT_SPEED = 5.0f;

	enum class OrbitMovement {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	struct orbit_camera {
		glm::vec3 target;
		float distance;

		float yaw;   // horizontal angle around target
		float pitch; // vertical angle

		float sensitivity;
		float zoom_speed;
		float min_distance;
		float max_distance;
		float fov;
		float movement_speed;

		orbit_camera(glm::vec3 target = glm::vec3(0.0f),
					float distance = ORBIT_DISTANCE,
					float yaw = ORBIT_YAW,
					float pitch = ORBIT_PITCH,
					float sensitivity = ORBIT_SENSITIVITY,
					float zoom_speed = ORBIT_ZOOM_SPEED,
					float min_distance = ORBIT_MIN_DIST,
					float max_distance = ORBIT_MAX_DIST,
					float fov = ORBIT_FOV, 
					float movement_speed = ORBIT_SPEED);

		glm::vec3 get_position() const;

		glm::mat4 get_view_matrix() const;

		void rotate(float xoffset, float yoffset);

		// Call from the GLFW scroll callback.
		void zoom(float yoffset);

		void pan(float xoffset, float yoffset);

		void frame(const glm::vec3& center, float boundingRadius);

		void move(OrbitMovement direction, float deltaTime) {
			float velocity = movement_speed * deltaTime;
			float yawRad = glm::radians(yaw);

			glm::vec3 forward = glm::normalize(target - get_position()); // true view direction
			glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

			if (direction == OrbitMovement::FORWARD)  target += forward * velocity;
			if (direction == OrbitMovement::BACKWARD) target -= forward * velocity;
			if (direction == OrbitMovement::LEFT)     target -= right * velocity;
			if (direction == OrbitMovement::RIGHT)    target += right * velocity;
		}
	};

}