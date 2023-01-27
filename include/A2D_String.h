#pragma once
#include <A2D_Core.h>
#include <string>

namespace A2D {

	template<class T>
	inline A2D::A2D_STRING_CLASS stringify(T arg) {
		throw 0;
	}

	template<>
	inline A2D::A2D_STRING_CLASS stringify(A2D::A2D_STRING_CLASS arg) {
		return arg;
	}

	template<>
	inline A2D::A2D_STRING_CLASS stringify(A2D::A2D_CONST_STRING arg) {
		return A2D::A2D_STRING_CLASS(arg);
	}

	template<>
	inline A2D::A2D_STRING_CLASS stringify(int arg) {
		return A2D_TO_STRING(arg);
	}

	template<class T, typename... Args>
	inline A2D::A2D_STRING_CLASS stringify(T first, Args... args) {
		return stringify(first) + stringify(args...);
	}
}