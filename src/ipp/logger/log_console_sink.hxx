#pragma once

#include <fstream>
#include <mutex>
#include <string>
#include <g3log/g3log.hpp>

namespace internal {
	class log_console_sink {
	private:
		std::mutex _lock;

	public:
		void log(g3::LogMessageMover mes);
	};
} /* logger */
