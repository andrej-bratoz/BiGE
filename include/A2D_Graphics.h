#pragma once

#include <A2D_Core.h>
#include <Windows.h>
#include <gdiplus.h>

namespace A2D
{
	 enum class ANDRO2D_API KnownColors : UINT32
	 {
		Red =					0xFFFF0000,
		Green =					0xFF00FF00,
		Blue =					0xFF0000FF,
	 	Transparent =			0x00FFFFFF,
	 	Control =				0xFFC0C0C0,
	 };
	
	struct ANDRO2D_API Color
	{
		Color(BYTE r, BYTE g, BYTE b) : m_alpha(255),
									    m_red(r),
									    m_green(g),
										m_blue(b)
		{}

		Color(BYTE alpha, BYTE r, BYTE g, BYTE b) : m_alpha(alpha),
													m_red(r),
													m_green(g),
													m_blue(b)
		{}

		[[nodiscard]] BYTE Alpha() const;
		[[nodiscard]] BYTE Red() const;
		[[nodiscard]] BYTE Green() const;
		[[nodiscard]] BYTE Blue() const;
		[[nodiscard]] static Color FromKnownColor(KnownColors c);
		
	private:
		BYTE m_alpha;
		BYTE m_red;
		BYTE m_green;
		BYTE m_blue;
	};
	
	class ANDRO2D_API Graphics
	{
	public:
		Graphics(HDC);
		Graphics(const Graphics&) = delete;
		Graphics& operator=(const Graphics&) = delete;
		void FillRectangle(Color c, int x, int y, int width, int height);
		
		[[nodiscard]] HDC GetHDC() const;
		~Graphics();
	private:
		HDC m_deviceCtx;
		Gdiplus::Graphics* _graphics;
	};
}


