#pragma once

#include <ctime>
#include <process.h>
#include <Windows.h>
#include <string>
#include <strsafe.h>
#include <gdiplus.h>
#include <memory>


#ifdef  ANDRO2D_LIB 
/*Enabled as "export" while compiling the dll project*/
#define ANDRO2D_API __declspec(dllexport)  
#else
/*Enabled as "import" in the Client side for using already created dll file*/
#define ANDRO2D_API __declspec(dllimport)  
#endif


namespace A2D
{
	class ANDRO2D_API IHandleObject
	{
	public:
		virtual ~IHandleObject() = default;
		virtual HWND GetHandle() const = 0;
	};

#if defined(UNICODE) || defined(_UNICODE) || defined(_MBCS)
	typedef wchar_t* A2D_STRING;
	typedef const wchar_t* A2D_CONST_STRING;
	#define A2D_TEXT(x) L##x
#else
	typedef char* A2D_STRING;
	typedef const char* A2D_CONST_STRING;
	#define A2D_TEXT(x) x
#endif

	namespace Internal
	{
		int GdiStartup();
		void GdiShutdown();

		template <typename  T>
		class List
		{
		public:
			List()
			{
				_size = 128;
				_data = static_cast<T*>(malloc(_size * sizeof(T)));
				_count = 0;
				RtlZeroMemory(_data, _size * sizeof(T));
			}

			List(const List& l)
			{
				_size = l._size;
				_data = l._data;
				_count = l._count;
			}

			void Add(T element)
			{
				_data[Count()] = element;
				_count++;
				if(_count == _size)
				{
					_size *= 2;
					_data = static_cast<T*>(realloc(_data, _size));
				}
			}

			int IndexOf(T element)
			{
				for(int i = 0; i < Count(); i++)
				{
					if (_data[i] == element) return i;
				}
				return -1;
			}

			void Get(int index, T* data)
			{
				*data = _data[index];
			}
			
			int Count() const { return _count; }

			int DeleteAt(int location)
			{
				if(location < Count() - 1)
				{
					for(int i = location; i < Count()-1; i++)
					{
						_data[i] = _data[i + 1];
					}
				}
				else if(location >= Count() )
				{
					return 0;
				}
				_count--;
				if(_size > _count*4)
				{
					_count = _count / 4;
					_data = static_cast<T*>(realloc(static_cast<void*>(_data), _count));
				}
				return 1;
			}

			~List()
			{
				if (_data == nullptr) return;
				free(_data);
				_data = nullptr;
			}

		private:
			int _count;
			int _size;
			T* _data;
		};

	}

	
}
