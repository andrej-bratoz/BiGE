
#include <a2d_controls/A2D_Button.h>

A2D::Controls::Button::Button(HWND parent) : Control(parent)
{}

A2D::Controls::Button::Button(const Button& btn) : Control(btn.m_parentHWnd)
{
	this->m_parentHWnd = btn.m_parentHWnd;
	this->m_hWnd = btn.m_hWnd;
}

void A2D::Controls::Button::SetText(A2D_CONST_STRING text) const
{
	SendMessage(this->m_hWnd, WM_SETTEXT, 0, (LPARAM)text);
}

void A2D::Controls::Button::Create()
{
	this->m_hWnd = CreateWindow(TEXT("BUTTON"),
		TEXT("OK"), 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		this->GetX(),
		this->GetY(),
		this->GetWidth(), 
		this->GetHeight(),
		this->m_parentHWnd,
		nullptr,
		(HINSTANCE)(GetWindowLongPtr(this->m_parentHWnd, GWLP_HINSTANCE)),
		nullptr);
}

A2D::Controls::Button::~Button() = default;
