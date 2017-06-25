#pragma once

#include "ipp/protocol/connection.hxx"
#include "ipp/protocol/message/message.hxx"

namespace ipp {
	namespace protocol {
		class protocol_listener {
		public:
			virtual void on_keep_alive(connection&, const message::keep_alive*, std::size_t len) = 0;
			virtual void on_connect(connection&, const message::connect*, std::size_t len) = 0;
			virtual void on_disconnect(connection&, const message::disconnect*, std::size_t len)= 0;

			virtual~protocol_listener() = 0;
		};

		inline protocol_listener::~protocol_listener() {}
	}
}
