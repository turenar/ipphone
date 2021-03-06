#include "ipp/logger/logger.hxx"
#include "ippbin/logger.hxx"
#include <g3log/logworker.hpp>
#include "ipp/version.hxx"
#include "ipp/logger/log_console_sink.hxx"

namespace ippbin {
	logger::logger() = default;
	logger::~logger() = default;

	void logger::install_logger() {
#ifdef NDEBUG
		g3::only_change_at_initialization::setLogLevel(DEBUG, false);
#endif
		g3::only_change_at_initialization::setLogLevel(ERROR, true);
		_worker = g3::LogWorker::createLogWorker();
//#ifdef _DEBUG
		_worker->addSink(std::make_unique<ipp::logger::log_console_sink>("log.txt"),
		                 &ipp::logger::log_console_sink::log);
		_worker->addSink(std::make_unique<ipp::logger::log_console_sink>(nullptr, ERROR.value),
		                 &ipp::logger::log_console_sink::log);
//#endif
		g3::initializeLogging(_worker.get());
		LOG(INFO) << "IPP Version: " << ipp::VERSION;
	}
}
