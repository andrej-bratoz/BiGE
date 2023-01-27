#include <A2D_Core.h>

Gdiplus::GdiplusStartupInput g_GDI_START_INPUT;
ULONG_PTR g_GDI_TOKEN;
std::vector<A2D::IWndProcObject*>* RegisteredWindows = new std::vector<A2D::IWndProcObject*>();

int A2D::Internal::GdiStartup()
{
	if (g_GDI_TOKEN != 0) return -1;
	Gdiplus::GdiplusStartup(&g_GDI_TOKEN, &g_GDI_START_INPUT, nullptr);
	return 0;
}

void A2D::Internal::GdiShutdown()
{
	Gdiplus::GdiplusShutdown(g_GDI_TOKEN);
	g_GDI_TOKEN = 0;
}

void A2D::DeleteWindow(IWndProcObject* c) {
	for (std::vector<IWndProcObject*>::iterator it = RegisteredWindows->begin(); it != RegisteredWindows->end();)
	{
		if (*it == c)
			it = RegisteredWindows->erase(it);
		else
			++it;
	}
}
