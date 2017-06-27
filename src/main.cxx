#include <iostream>
#include <boost/exception/diagnostic_information.hpp>
#include <g3log/logworker.hpp>
#include <ipp/protocol/connection_manager.hxx>
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

	class listener : public ipp::protocol::protocol_listener {
	public:
		virtual void
		on_connect(ipp::protocol::connection&, const ipp::protocol::message::connect*, std::size_t) override {
			LOG(INFO) << "msg=connect";
		}

		virtual void
		on_disconnect(ipp::protocol::connection&, const ipp::protocol::message::disconnect*, std::size_t) override {
			LOG(INFO) << "msg=disconnect";
		}

		virtual void
		on_keep_alive(ipp::protocol::connection&, const ipp::protocol::message::keep_alive*, std::size_t) override {
			LOG(INFO) << "msg=keepalive";
		}

		virtual void on_channel_open(ipp::protocol::connection&, const ipp::protocol::message::channel_open*,
		                             std::size_t) override {
			LOG(INFO) << "msg=channel_open";
		}

		virtual void on_channel_data(ipp::protocol::connection&, const ipp::protocol::message::channel_data*,
		                             std::size_t) override {
			LOG(INFO) << "msg=channel_data";
		}

		virtual void on_channel_close(ipp::protocol::connection&, const ipp::protocol::message::channel_close*,
		                              std::size_t) override {
			LOG(INFO) << "msg=channel_close";
		}
	};
}

int main() {
	auto worker = prepare_logger();
	try {
		ipp::network::socket rsock;
		ipp::network::socket_address addr = ipp::network::socket_address().set_address_any().set_port(50000u);
		rsock.bind(addr);
		listener l;
		ipp::protocol::connection_manager rman{std::move(rsock), l};

		ipp::network::socket wsock;
		ipp::protocol::connection wman{wsock.connect(addr), l};
		wman.protocol().connect();
		wman.protocol().keep_alive();
		wman.protocol().channel_open(1, ipp::protocol::message::channel_type::sound_stereo,
		                             ipp::protocol::message::channel_flag::receivable);
		std::uint8_t v[0x100] = {};
		wman.protocol().channel_data(1, v, sizeof(v));
		wman.protocol().channel_close(1);
		wman.protocol().disconnect();
		rman.consume_socket();
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
