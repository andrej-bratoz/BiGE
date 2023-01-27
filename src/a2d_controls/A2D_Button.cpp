
#include <a2d_controls/A2D_Button.h>
#include <A2D_Window.h>
#include <A2D_Core.h>
#include <vector>

extern std::vector<A2D::IWndProcObject*>* RegisteredWindows;
void Button_Paint(HWND hwnd);

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
	ANB_WNDCLASSEX wcex;
	wcex.cbSize = sizeof(ANB_WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = static_cast<WNDPROC>([](HWND hwnd, UINT uMsg , WPARAM wParam, LPARAM lParam) -> LRESULT
	{
		auto window = A2D::FindWnd(hwnd);
		if (window == nullptr) return DefWindowProc(hwnd, uMsg, wParam, lParam);
		return window->WndProc(hwnd, uMsg, wParam, lParam);
	});
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = GetModuleHandle(nullptr);
	wcex.hIcon = nullptr;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1));
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = A2D_TEXT("A2D_Button");
	wcex.hIconSm = nullptr;
	A2D_RegisterClassEx(&wcex);

	this->m_hWnd = A2D_CreateWidnow(wcex.lpszClassName,
		L"A2D_BUTTON",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		this->GetX(),
		this->GetY(),
		this->GetWidth(),
		this->GetHeight(),
		this->m_parentHWnd,
		nullptr,
		(HINSTANCE)(GetWindowLongPtr(this->m_parentHWnd, GWLP_HINSTANCE)),
		nullptr);

	RegisteredWindows->push_back(this);
}

LRESULT A2D::Controls::Button::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_PAINT && !this->m_eventHandlers->contains(WM_PAINT)) Button_Paint(hwnd);
	return A2D::Control::WndProc(hwnd, message, wParam, lParam);
}

A2D::Controls::Button::~Button() {
	DeleteWindow(this);
}

void Button_Paint(HWND hwnd) {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	// All painting occurs here, between BeginPaint and EndPaint.
	auto color = CreateSolidBrush(RGB(255, 0, 0));
	FillRect(hdc, &ps.rcPaint, (HBRUSH)(color));
	DeleteObject(color);
	EndPaint(hwnd, &ps);
}