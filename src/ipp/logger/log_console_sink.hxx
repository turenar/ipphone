#pragma once

#include <cstdio>
#include <mutex>
#include <g3log/g3log.hpp>

namespace ipp {
	namespace logger {
		class log_console_sink {
		public:
			log_console_sink(const char* = nullptr, int minimum_log_level = 0);
			void log(g3::LogMessageMover mes);

		private:
			std::mutex _lock;
			FILE* _fp;
			int _minimum_log_level;
		};
	} /* logger */
}
