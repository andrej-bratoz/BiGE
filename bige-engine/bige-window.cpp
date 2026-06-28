#include <GLFW/glfw3.h>
#include <bige-window.h>
#include <bige-camera.h>

namespace bige {
	Window::Window(int width, int height, const std::string& name) : 
		m_handle(nullptr),
		m_width(width), 
		m_height(height), 
		m_name(name) {
	}

	bool Window::create() {
		m_handle = glfwCreateWindow(m_width, m_height, m_name.c_str(), NULL, NULL);
		if (!m_handle) {
			return false;
		}
		glfwMakeContextCurrent(m_handle);
		glViewport(0, 0, m_width, m_height);
		//
		glfwSetFramebufferSizeCallback(m_handle, [](GLFWwindow* window, int width, int height) {
			glViewport(0, 0, width, height);
			bige::Window::width = width;
			bige::Window::height = height;
		});
		glfwSetMouseButtonCallback(m_handle, [](GLFWwindow* window, int button, int action, int mods) {
			if (!Window::g_wnd) return;
			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				Window::g_wnd->m_rotating = (action == GLFW_PRESS);
				if (Window::g_wnd->m_rotating) glfwGetCursorPos(window, &Window::g_wnd->m_last_x, &Window::g_wnd->m_last_y);
			}
			//
			if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
				Window::g_wnd->m_panning = (action == GLFW_PRESS);
				if (Window::g_wnd->m_panning) glfwGetCursorPos(window, &Window::g_wnd->m_last_x, &Window::g_wnd->m_last_y);
			}
		});
		glfwSetCursorPosCallback(m_handle, [](GLFWwindow* window, double xpos, double ypos) {
			if(!Window::g_wnd) return;
			float xoffset = static_cast<float>(xpos - Window::g_wnd->m_last_x);
			float yoffset = static_cast<float>(ypos - Window::g_wnd->m_last_y);
			Window::g_wnd->m_last_x = xpos;
			Window::g_wnd->m_last_y = ypos;

			if (Window::g_wnd->m_rotating) bige::orbit_camera::instance->rotate(xoffset, -yoffset);
			if (Window::g_wnd->m_panning)  bige::orbit_camera::instance->pan(xoffset, yoffset);
		});
		glfwSetScrollCallback(m_handle, [](GLFWwindow* window, double xoffset, double yoffset) {
			if (!Window::g_wnd) return;
			bige::orbit_camera::instance->zoom((float)yoffset);
		});
		return true;
	}

	void Window::set_background_color(float r, float g, float b, float a) {
		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	Window::~Window() {
		if (m_handle) {
			glfwDestroyWindow(m_handle);
		}
	}
}

