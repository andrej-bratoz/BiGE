#pragma once

#include <A2D_Core.h>
#include <exception>

namespace A2D
{
	class ANDRO2D_API Exception 
	{
	public:
		Exception() : m_Message("") {}
		Exception(const char* data);
		
	protected:
		virtual ~Exception() { delete m_Message; }
	private:
		const char* m_Message;
	};

	class ANDRO2D_API WindowException : public Exception
	{
	public:
		WindowException(const char* data) : Exception(data){}
	};
}

