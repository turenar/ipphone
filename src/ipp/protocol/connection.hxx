#pragma once

#include "ipp/network/socket_connection.hxx"
#include "ipp/protocol/ippp.hxx"
#include "ipp/util/buffer.hxx"

namespace ipp {
	namespace util {
		template <typename T>
		class buffer;
	}

	namespace protocol {
		class protocol_listener;

		class connection {
		public:
			using listener_type = protocol_listener;

			connection(network::socket_connection, listener_type&);

			bool parse_message(const std::uint8_t* msg, std::size_t len);
			ippp& protocol();

			listener_type& get_listener();
			void consume();

		private:
			std::uint8_t _buf[65536 * 2];
			std::size_t _remain_len = 0;
			ippp _protocol;
			network::socket_connection _con;
			listener_type& _listener;
		};
	}
}
