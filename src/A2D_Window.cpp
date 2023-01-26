#pragma once

#include <vector>
#include <A2D_Window.h>
#include <A2D_Exception.h>
#include <A2D_Graphics.h>

std::vector<std::shared_ptr<A2D::Window>> RegisteredWindows;

namespace A2D {

    A2D::Window* FindWnd(HWND hWnd)
    {
        for (const auto& win : RegisteredWindows)
        {
            if (win->GetHandle() == hWnd) return win;
        }
        return nullptr;
    }
}

static LRESULT CALLBACK A2D_WindowProc(
    _In_ HWND   hwnd,
    _In_ UINT   uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam
)
{
    auto window = A2D::FindWnd(hwnd);
    if (window == nullptr) return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    return window->WndProc(hwnd, uMsg, wParam, lParam);
}

void A2D::Window::InitializeInternal()
{
	m_hInstance = GetModuleHandleW(nullptr);
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = static_cast<WNDPROC>([](HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) -> LRESULT
    {
		return A2D_WindowProc(hwnd, message, wParam, lParam);
    });
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = m_hInstance;
    wcex.hIcon = nullptr;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1));
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"A2DClass";
    wcex.hIconSm = nullptr;
	const auto value = RegisterClassExW(&wcex);

	if(value == 0)
	{
        throw WindowException("RegisterClassEx failed");
	}
    this->m_Handle = CreateWindowW(wcex.lpszClassName, LPWSTR(this->m_title), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, m_hInstance, nullptr);
    if (!GetHandle()) throw WindowException("CreateWindow Failed");
    RegisteredWindows.push_back(this);
}

void A2D::Window::Show()
{
    if(!m_initialized)
    {
        this-> InitializeInternal();
        m_initialized = true;
    }

    ShowWindow(this->m_Handle, 0);
    UpdateWindow(this->m_Handle);
    SetWindowPos(this->m_Handle, HWND_TOPMOST, 0, 0, 
        this->m_width, this->m_height, SWP_SHOWWINDOW);
}


A2D::Window::Window(int width, int height, A2D_CONST_STRING title) : m_height(height),
																	 m_width(width),
																	 m_posX(0),
																	 m_posY(0),
																	 m_title(title),
																	 m_hInstance(nullptr),
																	 m_Handle(nullptr),
																	 m_initialized(false),
                                                                     m_callbacks(new std::map<UINT,A2D::A2D_WINDOW_MESSAGE_CALLBACK>)
{
    this->_controls = new ControlList;
}

A2D::Window::Window(const Window& w)
{
	this->m_width = w.m_width;
	this->m_height = w.m_height;
    this->m_posX = w.m_posX;
    this->m_posY = w.m_posY;
    this->m_title = w.m_title;
    this->m_hInstance = w.m_hInstance;
    this->m_Handle = w.m_Handle;
    this->m_initialized = w.m_initialized;
    this->_controls = w._controls;
    this->m_callbacks = new std::map<UINT, A2D::A2D_WINDOW_MESSAGE_CALLBACK>(*w.m_callbacks);
}

HWND A2D::Window::GetHandle() const
{
	return m_Handle;
}

LRESULT A2D::Window::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    WINDOWPOS* windowPos;

    if (this->m_callbacks->count(message)) {
        A2D::CallbackData cd{ .hwnd = hwnd, .message = message, .wParam = wParam, .lParam = lParam, .handled = false, .return_value = 0 };
        m_callbacks->at(message)(cd);
        if (cd.handled) return cd.return_value;
    }

	switch (message)
	{
		case WM_QUIT:
        case WM_DESTROY:
            DestroyWindow(this->GetHandle());
            RegisteredWindows.erase(
            std::remove_if(RegisteredWindows.begin(), 
                RegisteredWindows.end(), 
                [this](Window* w) -> bool
                {
                    return this->GetHandle() == w->GetHandle();
                }),
                RegisteredWindows.end());
         	PostQuitMessage(0);
			break;
        case WM_WINDOWPOSCHANGED: //https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-windowposchanged
            windowPos = reinterpret_cast<WINDOWPOS*>(lParam);
            this->m_posX = windowPos->x;
            this->m_posY = windowPos->y;
            this->SetDimension(windowPos->cx, windowPos->cy);
			OnPositionChanged();
            return 0;
        case WM_SIZE: // https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-size
            this->SetDimension(LOWORD(lParam), HIWORD(lParam));
            return 0;
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            this->OnPaint(hdc, ps);
            EndPaint(hwnd, &ps);
            break;
		default:
           return DefWindowProc(hwnd, message, wParam, lParam);
	}
    return DefWindowProc(hwnd, message, wParam, lParam);
}

int A2D::Window::GetHeight() const
{
    return this->m_height;
}

int A2D::Window::GetWidth() const
{
    return this->m_width;
}

void A2D::Window::SetDimension(int width, int height)
{
    BOOL requiredOnResize = false;
	if(this->m_height == height && this->m_width == width) {return;}
    this->m_height = height;
    this->m_width = width;
    OnResize();
}

int A2D::Window::GetPositionX() const
{
    return this->m_posX;
}

int A2D::Window::GetPositionY() const
{
    return this->m_posY;
}

void A2D::Window::AddControl(Control* c)
{
	this->_controls->Add(c);
    c->Create();
}

void A2D::Window::OnPaint(HDC hDC, const PAINTSTRUCT& ps)
{
    A2D::Graphics graphics(hDC);
    graphics.FillRectangle(Color::FromKnownColor(KnownColors::Control), 0, 0, this->GetWidth(), this->GetHeight());
	return;
}

void A2D::Window::OnResize()
{
    SetWindowPos(this->GetHandle(), nullptr, this->m_posX,
        this->m_posY, this->m_width, this->m_height, SWP_SHOWWINDOW);
}

void A2D::Window::OnPositionChanged()
{
    SetWindowPos(this->GetHandle(), nullptr, this->m_posX,
        this->m_posY, this->m_width, this->m_height, SWP_SHOWWINDOW);
}

void A2D::Window::RegisterCallback(UINT message, A2D_WINDOW_MESSAGE_CALLBACK cb)
{
    if (this->m_callbacks->count(message))
    {
        this->m_callbacks->erase(message);
    }
    this->m_callbacks->insert(std::make_pair(message, cb));
}