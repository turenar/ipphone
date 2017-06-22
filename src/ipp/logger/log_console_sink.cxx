#include "ipp/logger/log_console_sink.hxx"
#include <iostream>
#include "log_levels.hxx"

namespace ipp {
	namespace logger {
		void log_console_sink::log(g3::LogMessageMover mesinst) {
			std::lock_guard<std::mutex> lock(_lock);
			auto& mes = mesinst.get();

#ifdef __GNUC__
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
			std::cerr << "\033[" << levelColor << 'm' << mes.level() << '(' << mes.file() << ':' << mes.line()
			          << ")\033[0m: " << mes.message() << std::endl;
#else
			std::ostringstream buf;
			buf << mes.level() << '(' << mes.file() << ':' << mes.line() << "): " << mes.message() << '\n';
			std::string tmp(util::wtoa(utf8to16(buf.str())));
			OutputDebugStringA(tmp.c_str());
#endif
		}
	} /* logger */
}
