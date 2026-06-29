#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <bige-utils.h>
#include <bige-camera.h>

namespace bige {
	struct Window {
		Window(int width, int height, const std::string& name);
		bool create();
		void* handle() const { return m_handle; }
		void set_background_color(float r, float g, float b, float a = 1.0f);
		~Window();
		inline static std::unique_ptr<Window> engine_window = nullptr;
		int m_width;
		int m_height;
		bool m_block_events = false;
	private:
		GLFWwindow* m_handle;
		std::string m_name;
	};

// if underlying API  is OpenGL
#	define BIGE_WINDOW_HANDLE(x) (GLFWwindow*)(x)
}
