#include <iostream>
#include <boost/exception/diagnostic_information.hpp>
#include <g3log/logworker.hpp>
#include "ipp/version.hxx"
#include "ipp/logger/log_console_sink.hxx"
#include "ipp/logger/logger.hxx"
#include "ipp/network/socket.hxx"
#include "ipp/protocol/ippp.hxx"

namespace {
	auto prepare_logger() -> decltype(g3::LogWorker::createLogWorker()) {
#ifdef NDEBUG
		g3::only_change_at_initialization::setLogLevel(DEBUG, false);
#endif
		g3::only_change_at_initialization::setLogLevel(ERROR, true);
		auto worker = g3::LogWorker::createLogWorker();
//#ifdef _DEBUG
		worker->addSink(std::make_unique<ipp::logger::log_console_sink>(), &ipp::logger::log_console_sink::log);
//#endif
		g3::initializeLogging(worker.get());
		LOG(INFO) << "IPP Version: " << ipp::VERSION;
		return worker;
	}
}

int main() {
	auto worker = prepare_logger();
	try {
		ipp::network::socket rsock;
		rsock.bind(ipp::network::socket_address().set_address_any().set_port(0));
		ipp::network::socket_address listening = rsock.get_listening_address();
		LOG(DEBUG) << listening.get_address_str() << ':' << listening.get_port();

		char buf[65536];
		ipp::network::socket_address addr;
		std::size_t len = rsock.recv(reinterpret_cast<uint8_t*>(buf), sizeof(buf), addr);

		std::cout << "received: " << std::string(buf, len);

		ipp::network::socket_connection con = rsock.connect(addr);
		ipp::protocol::ippp p{std::move(con)};

		p.connect();
		p.disconnect();
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
