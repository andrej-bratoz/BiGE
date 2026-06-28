#pragma once
#include <bige-engine.h>
#include <string>
#include <sstream>
#include <fstream>
#include <chrono>
#include <format>

namespace bige {
	struct fmt_logger {
		fmt_logger() {
			ofs.open("engine.log", std::ios::out | std::ios::app);
			if (ofs.is_open()) {
				active = true;
			}
		}
		void close() {
			active = false;
			ofs.close();
		}

		void log(const std::string& message);
		std::string file_path = "bige.log";
		bool active = false;
		std::ofstream ofs;
	};

	struct logger {
		void log(const std::string& message) {
			std::stringstream ss;
			auto now = std::chrono::system_clock::now();
			std::time_t time_now = std::chrono::system_clock::to_time_t(now);
			std::tm tm_now = *std::localtime(&time_now);
			ss << std::put_time(&tm_now, "%Y-%m-%d %H:%M:%S") << " " << header << ": " << message;
			s_logger->log(ss.str());
		}
		std::string header;
		inline static std::unique_ptr<fmt_logger> s_logger = std::make_unique<fmt_logger>();
	};


#	define _log(x) bige::logger _bige_logger____ {.header = x}
#	define _l(x, ...) _bige_logger____.log(std::format(x, __VA_ARGS__))

#ifdef BIGE_TRACE_LOGGING_ENABLED
#	define _lt(x, ...) _bige_logger____.log(std::format(x, __VA_ARGS__))	
#else
#	define _lt(x, ...)
#endif
}