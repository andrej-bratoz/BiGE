#pragma once

#include <A2D_Core.h>
#include <a2d_controls/A2D_ControlBase.h>

namespace A2D
{
	namespace Controls
	{
		class ANDRO2D_API Button : public A2D::Control
		{
		public:
			Button(HWND parent);
			Button(const Button&);
			void SetText(A2D_CONST_STRING text) const;
			void Create() override;
			virtual LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override;
			virtual ~Button();
		protected:


		private:

		};
	}
}
