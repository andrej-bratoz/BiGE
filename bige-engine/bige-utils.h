#pragma once
#include <string>
#include <functional>
#include <random>
#include <cstdint>
#include <mutex>
#include <limits>

namespace bige {
	struct _defer_object {
		template <typename F>
		_defer_object(F&& f) : m_func(std::forward<F>(f)) {}
		~_defer_object() { m_func(); }
	private:
		std::function<void()> m_func;
	};

#	define BIGE_CONCAT_IMPL(x, y) x##y
#	define BIGE_CONCAT(x, y) BIGE_CONCAT_IMPL(x, y)
#	define BIGE_DEFER(code) bige::_defer_object BIGE_CONCAT(_defer_, __COUNTER__)([&](){code})

	template<typename T>
	inline std::string stringify(const T& arg) {
		return std::to_string(arg);
	}

	template<>
	inline std::string stringify(const std::string& arg) {
		return arg;
	}

	inline std::string stringify(const char* arg) {
		return arg;
	}

	template<typename T, typename... Args>
	inline std::string stringify(T first, Args... args) {
		return stringify(first) + stringify(args...);
	}

	inline uint64_t generate_random_id() {
		static std::mt19937_64 rng(std::random_device{}());
		static std::uniform_int_distribution<uint64_t> dist;
		return dist(rng);
	}

	std::string exe_path();
	std::string exe_dir();


	struct random {
		float next(float min, float max);
		int next(int min, int max);

		std::mt19937_64 m_random_engine{ std::random_device{}() };
		std::mutex m_random_mutex;
	}; 	
}