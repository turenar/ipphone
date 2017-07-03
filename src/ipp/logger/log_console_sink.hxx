#pragma once

#include <cstdio>
#include <mutex>
#include <g3log/g3log.hpp>

namespace ipp {
	namespace logger {
		class log_console_sink {
		public:
			log_console_sink(const char* = nullptr);
			void log(g3::LogMessageMover mes);

		private:
			std::mutex _lock;
			FILE* _fp;
		};
	} /* logger */
}
