#include <A2D_App.h>

int main() {

	A2D::Window w(300, 300, A2D_TEXT("HELLO"));
	w.Show();

	w.RegisterCallback(WM_WINDOWPOSCHANGED, [](const A2D::CallbackData& d) {
		OutputDebugStringA("POSITION CHANGING");
		OutputDebugStringA("\n");
	});

	A2D::A2DApp::Run(w);
	return 0;
}