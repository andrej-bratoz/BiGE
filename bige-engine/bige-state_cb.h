#pragma once
#include <bige-engine.h>

namespace bige {
	namespace state_cb {
		BIGE_ENGINE_DECLARE_STATE_CB(trace_logger);
		BIGE_ENGINE_DECLARE_STATE_CB(message_box);
		BIGE_ENGINE_DECLARE_STATE_PARAM_CB(window_color_callback, bige::engine*, engine);
		BIGE_ENGINE_DECLARE_STATE_CB(world_editor_callback);
	}
	namespace state {
		constexpr const char* MESSAGE_CB_SHOW = "bige.messagebox.show";
		//
		constexpr const char* LOGGER_CB_TRACE = "bige.logger.trace";
		constexpr const char* LOGGER_CB_INFO = "bige.logger.info";
		constexpr const char* LOGGER_CB_WARN = "bige.logger.warning";
		constexpr const char* LOGGER_CB_ERROR = "bige.logger.error";


		constexpr const char* LOGGER_LOGGING_LEVEL = "bige.logger.logging_level";
		
		// settings
		constexpr const char* SYSTEM_NO_OF_MONITORS = "bige.system.number_of_monitors";
		constexpr const char* SYSTEM_UPTIME_START = "bige.system.uptime_start";
		constexpr const char* SYSTEM_OS_NAME = "bige.system.os_name";
		constexpr const char* SYSTEM_CPU_MODEL = "bige.system.cpu_model";
		constexpr const char* SYSTEM_GPU_MODEL = "bige.system.gpu_model";
		constexpr const char* SYSTEM_TOTAL_RAM = "bige.system.total_ram";
		constexpr const char* SYSTEM_AVAILABLE_RAM = "bige.system.available_ram";
		constexpr const char* SYSTEM_DISK_SPACE = "bige.system.disk_space";

		// window
		constexpr const char* WINDOW_BACKGROUND_COLOR = "bige.window.background_color";

		//engine
		constexpr const char* ENGINE_FPS = "bige.engine.fps";
	}
}