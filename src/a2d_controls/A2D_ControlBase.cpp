#include <a2d_controls/A2D_ControlBase.h>
#include <a2d_controls/A2D_ControlBase.h>
#include <A2D_Window.h>
#include <ranges>

A2D::Control::Control(HWND parent) : m_parentHWnd(nullptr),
								m_hWnd(nullptr),
								m_posX(0),
								m_posY(0),
								m_width(10),
								m_height(10),
								m_eventHandlers(new std::multimap<UINT, A2D_WINDOW_MESSAGE_CALLBACK>())
{
	this->_controls = new ControlList();
	this->m_parentHWnd = parent;
}

A2D::Control::Control(const A2D::Control& c)
{
	this->m_parentHWnd = c.m_parentHWnd;
	this->m_hWnd = c.m_hWnd;
	this->m_posX = c.m_posX;
	this->m_posY = c.m_posY;
	this->m_width = c.m_width;
	this->m_height = c.m_height;
	this->_controls = c._controls;
	this->m_eventHandlers = new std::multimap<UINT, A2D_WINDOW_MESSAGE_CALLBACK>(*c.m_eventHandlers);
}

int A2D::Control::GetX() const
{
	return this->m_posX;
}

int A2D::Control::GetY() const
{
	return this->m_posY;
}

int A2D::Control::GetWidth() const
{
	return this->m_width;
}

int A2D::Control::GetHeight() const
{
	return this->m_height;
}

HWND A2D::Control::GetParent() const
{
	return this->m_parentHWnd;
}

HWND A2D::Control::GetHandle() const
{
	return this->m_hWnd;
}

void A2D::Control::SetPosition(const int x, const int y)
{
	this->m_posX = x;
	this->m_posY = y;
}

void A2D::Control::SetDimension(int width, int height)
{
	this->m_width = width;
	this->m_height = height;
}

A2D::Control::operator HWND() const
{
	return this->m_hWnd;
}

void A2D::Control::RegisterCallback(UINT message, A2D_WINDOW_MESSAGE_CALLBACK c) {
	this->m_eventHandlers->insert(std::make_pair(message, c));
}

LRESULT A2D::Control::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (this->m_eventHandlers->count(message)) {
		A2D::CallbackData cd{ .hwnd = hwnd, .message = message, .wParam = wParam, .lParam = lParam, .handled = false, .return_value = 0, .win = this };
		auto data = this->m_eventHandlers->equal_range(message);

		for (const auto& x : *(this->m_eventHandlers)) {
			if (x.first == message) {
				x.second(cd);
				if (cd.handled) return cd.return_value;
			}
		}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
