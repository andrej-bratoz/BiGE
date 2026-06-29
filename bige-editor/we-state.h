#pragma once
#include <memory>

namespace we {
	struct editor_state {
		double m_last_x = 0.0;
		double m_last_y = 0.0;
		bool m_rotating = false;
		bool m_panning = false;
		inline static std::unique_ptr<editor_state> instance = std::make_unique<editor_state>();
	};
}