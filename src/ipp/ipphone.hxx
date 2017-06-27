#pragma once

#include "ipp/protocol/connection_manager.hxx"
#include "ipp/protocol/protocol_listener.hxx"
#include "ipp/protocol/message/message_channel.hxx"
#include "ipp/protocol/message/message_connection.hxx"
#include "sox/format.hxx"

namespace ipp {
	class ipphone : public protocol::protocol_listener {
	public:
		ipphone();
		virtual ~ipphone();

		void bind(const std::string& ip, std::uint16_t port);
		void connect(const std::string& ip, std::uint16_t port);
		void update_frame(bool listen);

		virtual void on_keep_alive(protocol::connection&, const protocol::message::keep_alive*, std::size_t) override;
		virtual void on_connect(protocol::connection&, const protocol::message::connect*, std::size_t) override;
		virtual void on_disconnect(protocol::connection&, const protocol::message::disconnect*, std::size_t) override;

		virtual void
		on_channel_open(protocol::connection&, const protocol::message::channel_open*, std::size_t) override;
		virtual void
		on_channel_data(protocol::connection&, const protocol::message::channel_data*, std::size_t) override;
		virtual void
		on_channel_close(protocol::connection&, const protocol::message::channel_close*, std::size_t) override;

	private:
		protocol::connection_manager _manager;
		sox::format _input;
	};
}
