#include <csignal>
#include <iostream>
#include <locale.h>
#include <string>
#include <time.h>
#include <vector>
#include <boost/exception/diagnostic_information.hpp>
#include "ippbin/logger.hxx"
#include "ippbin/terminal.hxx"

int main() {
	signal(SIGPIPE, SIG_IGN);
	setlocale(LC_ALL, "");

	ippbin::logger l;
	l.install_logger();
	try {
		ippbin::terminal t;
		t.run();
	} catch (boost::exception& ex) {
		LOG(ERROR) << "uncaught exception";
		std::exception* stdex = dynamic_cast<std::exception*>(&ex);
		if (stdex) {
			LOG(ERROR) << "what: " << stdex->what();
		}
		LOG(ERROR) << boost::diagnostic_information_what(ex);
		return 1;
	} catch (std::exception& ex) {
		LOG(ERROR) << "uncaught exception";
		LOG(ERROR) << "what: " << ex.what();
		return 1;
	}
}

