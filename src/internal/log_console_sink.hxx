#pragma once

#include <fstream>
#include <string>
#include <mutex>
#include <g3log/g3log.hpp>

namespace internal {
	class log_console_sink {
	private:
		std::mutex _lock;

	public:
		void log(g3::LogMessageMover mes);
	};
} /* internal */
