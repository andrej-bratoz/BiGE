#include <a2d_controls/A2D_ControlBase.h>

Control::Control(HWND parent) : m_parentHWnd(nullptr),
								m_hWnd(nullptr),
								m_posX(0),
								m_posY(0),
								m_width(10),
								m_height(10)
{
	this->_controls = new ControlList();
	this->m_parentHWnd = parent;
}

Control::Control(const Control& c)
{
	this->m_parentHWnd = c.m_parentHWnd;
	this->m_hWnd = c.m_hWnd;
	this->m_posX = c.m_posX;
	this->m_posY = c.m_posY;
	this->m_width = c.m_width;
	this->m_height = c.m_height;
	this->_controls = c._controls;
}

int Control::GetX() const
{
	return this->m_posX;
}

int Control::GetY() const
{
	return this->m_posY;
}

int Control::GetWidth() const
{
	return this->m_width;
}

int Control::GetHeight() const
{
	return this->m_height;
}

HWND Control::GetParent() const
{
	return this->m_parentHWnd;
}

HWND Control::GetHandle() const
{
	return this->m_hWnd;
}

void Control::SetPosition(const int x, const int y)
{
	this->m_posX = x;
	this->m_posY = y;
}

void Control::SetDimension(int width, int height)
{
	this->m_width = width;
	this->m_height = height;
}

void Control::Destroy()
{
	if (this->m_hWnd != nullptr) 
	{
		DestroyWindow(this->m_hWnd);
	}
}

Control::operator HWND() const
{
	return this->m_hWnd;
}

