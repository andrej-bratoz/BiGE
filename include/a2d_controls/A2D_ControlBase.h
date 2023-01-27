#pragma once

#include <A2D_Core.h>
#include <map>

namespace A2D {
	
	class Control;
	typedef std::vector<A2D::Control*> ControlList;
	//
	class ANDRO2D_API Control : public IWndProcObject
	{
	public:
		explicit Control(HWND parent);
		Control(const Control& c);
		[[nodiscard]] int GetX() const;
		[[nodiscard]] int GetY() const;
		[[nodiscard]] int GetWidth() const;
		[[nodiscard]] int GetHeight() const;

		[[nodiscard]] HWND GetParent() const;
		[[nodiscard]] HWND GetHandle() const;

		void SetPosition(int x, int y);
		void SetDimension(int width, int height);
		virtual void Create() = 0;
		operator HWND() const;
		virtual LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override;
		virtual void RegisterCallback(UINT message, A2D_WINDOW_MESSAGE_CALLBACK cb);
		[[nodiscard]] const A2D::ControlList* Controls() const { return _controls; }

		//
		virtual ~Control() noexcept { 
			if (this->m_hWnd != nullptr)
			{
				DestroyWindow(this->m_hWnd);
			}
			delete _controls;
			delete m_eventHandlers;
		}
	protected:
		HWND m_parentHWnd;
		HWND m_hWnd;
		std::multimap<UINT, A2D_WINDOW_MESSAGE_CALLBACK>* m_eventHandlers;
	private:

		int m_posX;
		int m_posY;
		//
		int m_width;
		int m_height;
		A2D::ControlList* _controls;

		//	

	};
}



