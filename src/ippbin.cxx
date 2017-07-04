#include <iostream>
#include <boost/exception/diagnostic_information.hpp>
#include <g3log/logworker.hpp>
#include "ipp/channel/video_encoder_channel.hxx"
#include "ipp/ipphone.hxx"
#include "ipp/version.hxx"
#include "ipp/channel/file_channel.hxx"
#include "ipp/logger/log_console_sink.hxx"
#include "ipp/logger/logger.hxx"
#include "ipp/network/socket.hxx"
#include "ipp/protocol/connection_manager.hxx"
#include "ipp/protocol/ippp.hxx"
#include "ipp/protocol/protocol_listener.hxx"
#include "ipp/util/buffer.hxx"

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
	signal(SIGPIPE, SIG_IGN);

	ipp::util::buffer<std::uint16_t> buf(4);
	try {
		if (argc >= 3) {
			ipp::ipphone ip(true);
			ip.connect(argv[1], std::stoi(argv[2]));
			ip.open_channel(ipp::protocol::channel::channel_type::sound, ipp::protocol::channel::channel_flag::none);
			ip.open_channel(
					std::make_unique<ipp::channel::file_channel>(ip, 0, ipp::protocol::channel::channel_type::file,
					                                             "sendfile.jpeg"),
					ipp::protocol::channel::channel_flag::none);
			while (true) {
				ip.update_frame();
			}
		} else if (argc >= 2) {
			ipp::ipphone ip(true);
			ip.connect("127.0.0.1", 12345);
			ip.open_channel(ipp::protocol::channel::channel_type::sound, ipp::protocol::channel::channel_flag::none);
			ip.open_channel(
					std::make_unique<ipp::channel::video_encoder_channel>(
							ip, 0, ipp::protocol::channel::channel_type::mpeg2video),
					ipp::protocol::channel::channel_flag::none);
			while (true) {
				ip.update_frame();
			}
		} else {
			ipp::ipphone ip(true);
			ip.bind("0.0.0.0", 12345);
			while (true) {
				ip.update_frame();
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
