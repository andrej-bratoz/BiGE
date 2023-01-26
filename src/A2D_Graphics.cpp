#include <A2D_Graphics.h>

BYTE A2D::Color::Alpha() const
{
	return m_alpha; 
}

BYTE A2D::Color::Red() const
{
	return m_red; 
}

BYTE A2D::Color::Green() const
{
	return m_green; 
}

BYTE A2D::Color::Blue() const
{
	return m_blue;
}

A2D::Color A2D::Color::FromKnownColor(KnownColors c)
{
	const auto color = static_cast<UINT32>(c);
	const BYTE alpha = ((color & 0xFF000000) >> 24);
	const BYTE red = ((color & 0x00FF0000) >> 16);
	const BYTE green = ((color & 0x0000FF00) >> 8);
	const BYTE blue = ((color & 0x000000FF));

	const Color col(alpha, red, green, blue);
	return col;
}

//

A2D::Graphics::Graphics(HDC deviceCtx)
						: m_deviceCtx(deviceCtx)
{
	this->_graphics = new Gdiplus::Graphics(deviceCtx);
}

void A2D::Graphics::FillRectangle(const Color c, int x, int y, int width, int height)
{
	Gdiplus::SolidBrush brush(Gdiplus::Color(c.Alpha(), c.Red(), c.Green(), c.Blue()));
	this->_graphics->FillRectangle(&brush, x, y, width, height);
}

HDC A2D::Graphics::GetHDC() const
{
	return this->m_deviceCtx;
}

A2D::Graphics::~Graphics()
{
	delete this->_graphics;
}
