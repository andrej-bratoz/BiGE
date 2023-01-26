#pragma once

#include <A2D_Core.h>


class Control;

class ANDRO2D_API ControlList : public A2D::Internal::List<Control*>
{

};

class ANDRO2D_API Control
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
	virtual void Destroy();
	operator HWND() const;

	[[nodiscard]] const ControlList* Controls() const { return _controls; }
	
	//
	virtual ~Control() { delete _controls; }
protected:
	HWND m_parentHWnd;
	HWND m_hWnd;
private:

	int m_posX;
	int m_posY;
	//
	int m_width;
	int m_height;
	ControlList* _controls;

	//	

};



