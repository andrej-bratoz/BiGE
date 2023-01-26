#pragma once

#include <A2D_Core.h>
#include <a2d_controls/A2D_ControlBase.h>

namespace A2D
{
	namespace Controls
	{
		
		class ANDRO2D_API Button : public Control
		{
		public:
			Button(HWND parent);
			Button(const Button&);
			void SetText(A2D_CONST_STRING text) const;
			void Create() override;
			virtual ~Button();
		protected:


		private:

		};
	}
}
