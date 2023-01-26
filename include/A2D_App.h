#pragma once

#include <A2D_Window.h>
#include <memory>
#include <Windows.h>
#include <vector>

namespace A2D
{
	class ANDRO2D_API A2DApp
	{
	public:
		static WPARAM Run(const Window& target);
		static WPARAM Run();
	};

	
}


