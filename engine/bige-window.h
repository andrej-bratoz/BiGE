#pragma once
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
		inline static Window* g_wnd = nullptr;
		inline static std::unique_ptr<bige::orbit_camera> m_camera;
		inline static int width;
		inline static int height;
	private:
		GLFWwindow* m_handle;
		int m_width, m_height;
		std::string m_name;
		bool m_rotating = false;
		double m_last_x = 0.0, m_last_y = 0.0;
		bool m_panning = false;

	};

// if underlying API  is OpenGL
#	define BIGE_WINDOW_HANDLE(x) (GLFWwindow*)(x)
}
