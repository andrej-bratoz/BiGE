#include <A2D_App.h>

WPARAM ANDRO2D_API A2D::A2DApp::Run(const Window& target)
{
	A2D::Internal::GdiStartup();
	MSG msg;
	while (GetMessage(&msg,target.GetHandle(),0,0))
	{
		if (msg.message == 0) return 0;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	A2D::Internal::GdiShutdown();
	return static_cast<int>(msg.wParam);
}

WPARAM ANDRO2D_API A2D::A2DApp::Run()
{
	int result = A2D::Internal::GdiStartup();
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	A2D::Internal::GdiShutdown();
	return static_cast<int>(msg.wParam);
}