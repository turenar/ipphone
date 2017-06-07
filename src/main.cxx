#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include <iostream>
#include "internal/log_console_sink.hxx"

namespace {
	auto prepare_logger() -> decltype(g3::LogWorker::createLogWorker()) {
		auto worker = g3::LogWorker::createLogWorker();
//#ifdef _DEBUG
		worker->addSink(std::make_unique<internal::log_console_sink>(), &internal::log_console_sink::log);
//#endif
		g3::initializeLogging(worker.get());
		LOG(INFO) << "Hello, g3log!";
		return worker;
	}
}

int main() {
	auto worker = prepare_logger();
	std::cout << "Hello world!" << std::endl;
}
