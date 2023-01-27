#include <A2D_App.h>
#include <A2D_String.h>
#include <A2D_Window.h>

int main() {

	A2D::Window w(300, 300, A2D_TEXT("HELLO"));
	w.Show();

	A2D::Controls::Button b(w.GetHandle());
	b.SetDimension(200, 40);
	b.SetPosition(0, 0);
	b.SetText(A2D_TEXT("Press me!"));
	b.RegisterCallback(WM_LBUTTONUP, [](A2D::CallbackData& c) {
		MessageBox(nullptr, TEXT("Clicked"), TEXT("Cliecked"), MB_OK);
	});
	w.AddControl(&b);

	A2D::Controls::Button b2(w.GetHandle());
	b2.SetDimension(200, 40);
	b2.SetPosition(0, 50);
	b2.SetText(A2D_TEXT("Press me!"));
	b2.RegisterCallback(WM_PAINT, [](A2D::CallbackData& c) {
		c.handled = true;
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(c.hwnd, &ps);
		// All painting occurs here, between BeginPaint and EndPaint.
		auto color = CreateSolidBrush(RGB(0, 255, 0));
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(color));
		DeleteObject(color);
		EndPaint(c.hwnd, &ps);
		c.return_value = 0;
	});
	w.AddControl(&b2);

	w.RegisterCallback(WM_WINDOWPOSCHANGED, [](A2D::CallbackData& d) {
		A2D::DebugOut(A2D::stringify(
			A2D_TEXT("Window pos: "),((A2D::Window*)d.win)->GetPositionX(), A2D_TEXT(":"), 
			((A2D::Window*)d.win)->GetPositionY(), A2D_TEXT("\n")));
	});

	A2D::A2DApp::Run(w);
	return 0;
}