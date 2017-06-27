#pragma once

#include "ipp/protocol/connection.hxx"
#include "ipp/protocol/message/message.hxx"
#include "ipp/protocol/message/message_channel.hxx"
#include "ipp/protocol/message/message_connection.hxx"

namespace ipp {
	namespace protocol {
		class protocol_listener {
		public:
			virtual void on_keep_alive(connection&, const message::keep_alive*, std::size_t len) = 0;
			virtual void on_connect(connection&, const message::connect*, std::size_t len) = 0;
			virtual void on_disconnect(connection&, const message::disconnect*, std::size_t len) = 0;

			virtual void on_channel_open(connection&, const message::channel_open*, std::size_t len) = 0;
			virtual void on_channel_data(connection&, const message::channel_data*, std::size_t len) = 0;
			virtual void on_channel_close(connection&, const message::channel_close*, std::size_t len) = 0;

			virtual ~protocol_listener() = 0;
		};

		inline protocol_listener::~protocol_listener() {}
	}
}
