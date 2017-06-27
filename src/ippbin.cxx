#include <iostream>
#include <boost/exception/diagnostic_information.hpp>
#include <g3log/logworker.hpp>
#include <ipp/protocol/connection_manager.hxx>
#include "ipp/ipphone.hxx"
#include "ipp/version.hxx"
#include "ipp/logger/log_console_sink.hxx"
#include "ipp/logger/logger.hxx"
#include "ipp/network/socket.hxx"
#include "ipp/protocol/ippp.hxx"
#include "ipp/protocol/protocol_listener.hxx"

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

int main(int argc, char** argv) {
	auto worker = prepare_logger();
	try {
		if (argc >= 2) {
			ipp::ipphone ip;
			ip.connect("127.0.0.1", 12345);
			while (true) {
				ip.update_frame(false);
			}
		} else {
			ipp::ipphone ip;
			ip.bind("127.0.0.1", 12345);
			while (true) {
				ip.update_frame(true);
			}
		}
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
