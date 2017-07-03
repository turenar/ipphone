#include "ipp/logger/log_console_sink.hxx"
#include <iostream>
#include "log_levels.hxx"

namespace ipp {
	namespace logger {
		log_console_sink::log_console_sink(const char* filename) {
			if (filename) {
				_fp = fopen(filename, "w");
			} else {
				_fp = stderr;
			}
		}

		void log_console_sink::log(g3::LogMessageMover mesinst) {
			std::lock_guard<std::mutex> lock(_lock);
			auto& mes = mesinst.get();

			const char* levelColor;
			if (mes.wasFatal()) {
				levelColor = "1;30;41"; // bold black on red
			} else if (mes._level == ERROR) {
				levelColor = "1;31"; // bold red
			} else if (mes._level == WARNING) {
				levelColor = "1;33"; // bold yellow
			} else if (mes._level == INFO) {
				levelColor = "32"; // green
			} else {
				levelColor = "36"; // cyan
			}
			fprintf(_fp, "\033[%sm%s(%s:%s)\033[0m: %s\n", levelColor, mes.level().c_str(), mes.file().c_str(),
			        mes.line().c_str(), mes.message().c_str());
			fflush(_fp);
		}
	} /* logger */
}
