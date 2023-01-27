#pragma once

#include <ctime>
#include <process.h>
#include <Windows.h>
#include <string>
#include <strsafe.h>
#include <gdiplus.h>
#include <memory>
#include <vector>


#ifdef  ANDRO2D_LIB 
/*Enabled as "export" while compiling the dll project*/
#define ANDRO2D_API __declspec(dllexport)  
#else
/*Enabled as "import" in the Client side for using already created dll file*/
#define ANDRO2D_API __declspec(dllimport)  
#endif


namespace A2D
{
	class IWndProcObject;
	typedef struct ANDRO2D_API _CallbackData {
		HWND hwnd;
		UINT message;
		WPARAM wParam;
		LPARAM lParam;
		bool handled;
		LRESULT return_value;
		A2D::IWndProcObject* win;
	}CallbackData;

	typedef void (*A2D_WINDOW_MESSAGE_CALLBACK)(CallbackData& cb);

	class ANDRO2D_API IHandleObject
	{
	public:
		virtual ~IHandleObject() = default;
		virtual HWND GetHandle() const = 0;
	};

	class ANDRO2D_API IWndProcObject : public IHandleObject {
	public:
		virtual LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
		virtual void RegisterCallback(UINT message, A2D_WINDOW_MESSAGE_CALLBACK cb) = 0;
		virtual ~IWndProcObject() {}
	};


#if defined(UNICODE) || defined(_UNICODE) || defined(_MBCS)
	typedef wchar_t* A2D_STRING;
	typedef std::wstring A2D_STRING_CLASS;
	typedef const wchar_t* A2D_CONST_STRING;
	#define A2D_TO_STRING std::to_wstring
	#define A2D_TEXT(x) L##x
	#define A2D_DEBUG OutputDebugStringW
	typedef WNDCLASSEXW ANB_WNDCLASSEX;
	#define A2D_CreateWidnow CreateWindowW
	#define A2D_RegisterClassEx RegisterClassExW

#else
	typedef char* A2D_STRING;
	typedef const char* A2D_CONST_STRING;
	typedef std::string A2D_STRING_CLASS;
	#define A2D_TO_STRING std::to_string
	#define A2D_TEXT(x) x
	#define A2D_DEBUG OutputDebugStringA
	typedef WNDCLASSEXW ANB_WNDCLASSEX;
	#define A2D_CreateWidnow CreateWindowA
	#define A2D_RegisterClassEx RegisterClassExA
#endif

	#define CLASS_NAME(x) A2D_TEXT(#x) 

	namespace Internal
	{
		int GdiStartup();
		void GdiShutdown();
	}
	
	inline void DebugOut(const A2D_STRING_CLASS& cls) {
		A2D_DEBUG(cls.c_str());
	}	

	void DeleteWindow(IWndProcObject* c);
}
