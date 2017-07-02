#pragma once

#include "ipp/network/socket_connection.hxx"
#include "ipp/protocol/ippp.hxx"
#include "ipp/util/buffer.hxx"

namespace ipp {
	namespace protocol {
		class protocol_listener;

		class connection {
		public:
			using listener_type = protocol_listener;

			connection(network::socket_connection, listener_type&);

			void consume();
			bool parse_message(const std::uint8_t* msg, std::size_t len);
			ippp& protocol();

			listener_type& get_listener();


		private:
			ippp _protocol;
			network::socket_connection _con;
			util::buffer<std::uint8_t> _buf;
			listener_type& _listener;
		};
	}
}
