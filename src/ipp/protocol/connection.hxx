#pragma once

#include "ipp/network/socket_connection.hxx"
#include "ipp/protocol/ippp.hxx"

namespace ipp {
	namespace protocol {
		class connection {
		public:
			connection(network::socket_connection&&);

			void parse_message(const std::uint8_t* msg, std::size_t len);
			ippp& protocol();

		private:
			ippp _protocol;
		};
	}
}
