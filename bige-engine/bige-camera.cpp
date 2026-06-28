#include <bige-camera.h>

namespace bige {
	
	orbit_camera::orbit_camera(glm::vec3 target, float distance, float yaw, float pitch, float sensitivity, 
							   float zoom_speed, float min_distance, float max_distance, float fov, float mov_speed) :
		target(target), distance(distance), yaw(yaw), pitch(pitch),
		sensitivity(sensitivity), zoom_speed(zoom_speed),
		min_distance(min_distance), max_distance(max_distance), fov(fov), movement_speed(mov_speed) {
	}
	glm::vec3 orbit_camera::get_position() const {
		float yawRad = glm::radians(yaw);
		float pitchRad = glm::radians(pitch);

		glm::vec3 offset;
		offset.x = distance * cos(pitchRad) * cos(yawRad);
		offset.y = distance * sin(pitchRad);
		offset.z = distance * cos(pitchRad) * sin(yawRad);

		return target + offset;
	}
	
	glm::mat4 orbit_camera::get_view_matrix() const {
		return glm::lookAt(get_position(), target, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	
	void orbit_camera::rotate(float xoffset, float yoffset) {
		yaw += xoffset * sensitivity;
		pitch += yoffset * sensitivity;
		pitch = std::clamp(pitch, ORBIT_MIN_PITCH, ORBIT_MAX_PITCH);
	}

	void orbit_camera::zoom(float yoffset) {
		distance -= yoffset * zoom_speed;
		distance = std::clamp(distance, min_distance, max_distance);
	}
	
	void orbit_camera::pan(float xoffset, float yoffset) {
		glm::vec3 pos = get_position();
		glm::vec3 forward = glm::normalize(target - pos);
		glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
		glm::vec3 up = glm::normalize(glm::cross(right, forward));

		float panSpeed = distance * 0.001f; // scale with distance so it feels consistent
		target += -right * xoffset * panSpeed + up * yoffset * panSpeed;
	}
	
	void orbit_camera::frame(const glm::vec3& center, float boundingRadius) {
		target = center;
		distance = boundingRadius * 2.5f;
		min_distance = boundingRadius * 0.2f;
		max_distance = boundingRadius * 10.0f;
	}
}