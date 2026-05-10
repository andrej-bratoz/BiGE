#include <bige-logging.h>
#include <iostream>
#ifdef WIN32
#	include <Windows.h>
#endif

namespace bige {
	void fmt_logger::log(const std::string& message) {
#ifdef _DEBUG
#	ifdef WIN32
		std::stringstream ss;
		ss << message << "\n";
		std::string output = ss.str();
		OutputDebugStringA(output.c_str());
#	else
		std::cerr << output << "\n";
#	endif
#endif
		if (active) {
			ofs << message << std::endl;
		}
	}
}