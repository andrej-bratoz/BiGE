#include <string>
#include <filesystem>
#ifdef WIN32
#include <windows.h>
#include "bige-utils.h"
#endif

namespace bige {
	std::string exe_path() {
#ifdef WIN32
		char m_buffer[MAX_PATH];
		GetModuleFileNameA(NULL, m_buffer, MAX_PATH);
		return std::string(m_buffer);
#else
		throw 1;
#endif
	}

	std::string exe_dir() {
		std::filesystem::path path(bige::exe_path());
		return path.parent_path().make_preferred().generic_string();
	}

#undef min
#undef max
	float random::next(float min, float max) {
		std::lock_guard<std::mutex> lock(m_random_mutex);
		std::uniform_real_distribution<float> dist(min, max);
		return dist(m_random_engine);
	}

	int random::next(int min, int max) {
		std::lock_guard<std::mutex> lock(m_random_mutex);
		std::uniform_int_distribution<int> dist(min, max);
		return dist(m_random_engine);
	}
}