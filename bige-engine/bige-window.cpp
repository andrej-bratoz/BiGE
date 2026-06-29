#include <bige-window.h>
#include <bige-camera.h>
#ifdef BIGE_ENGINE_HAS_EDITOR
#	include <we-input-events.h>
#endif

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
		//
		glfwSetFramebufferSizeCallback(m_handle, [](GLFWwindow* window, int width, int height) {
			glViewport(0, 0, width, height);
			bige::Window::engine_window->m_width = width;
			bige::Window::engine_window->m_height = height;
		});
#ifdef BIGE_ENGINE_HAS_EDITOR
		// since only called once, we can "leak" the memory here, as it will be used for the lifetime of the application
		new we::register_editor_input_events(m_handle);
#endif
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

