#include <iostream>
#include <boost/exception/diagnostic_information.hpp>
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include "log_levels.hxx"
#include "version.hxx"
#include "logger/log_console_sink.hxx"
#include "network/socket.hxx"

namespace {
	auto prepare_logger() -> decltype(g3::LogWorker::createLogWorker()) {
		auto worker = g3::LogWorker::createLogWorker();
//#ifdef _DEBUG
		worker->addSink(std::make_unique<internal::log_console_sink>(), &internal::log_console_sink::log);
//#endif
		g3::initializeLogging(worker.get());
		LOG(INFO) << "IPP Version: " << ipp::VERSION;
		return worker;
	}
}

int main() {
	auto worker = prepare_logger();
	try {
		ipp::network::socket_address addr;
		addr.set_address("127.0.0.1").set_port(80u);

		ipp::network::socket sock;
		ipp::network::socket_connection con{sock.connect(addr)};

		const char r[] = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n";
		con.send(reinterpret_cast<const std::uint8_t*>(r), sizeof(r) - 1);

		while (true) {
			char buf[4096];
			std::size_t s = con.recv(reinterpret_cast<std::uint8_t*>(buf), sizeof(buf));
			if (s == 0) {
				break;
			}
			std::cout << std::string(buf, s);
		}
	} catch (boost::exception& ex) {
		LOG(ERROR) << "uncaught exception";
		std::exception* stdex = dynamic_cast<std::exception*>(&ex);
		if (stdex) {
			LOG(ERROR) << "what: " << stdex->what();
		}
		LOG(ERROR) << boost::diagnostic_information_what(ex);
		return 1;
	}
}
