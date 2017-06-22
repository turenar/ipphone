#pragma once

#include <memory>
#include "ipp/network/socket_connection.hxx"
#include "ipp/protocol/packet/packet_builder.hxx"
#include "packet/packet.hxx"

namespace ipp {
	namespace protocol {
		class ippp {
		public:
			static constexpr std::uint8_t protocol_version = 0;
			static constexpr std::uint8_t protocol_revision = 1;

			ippp(network::socket_connection&&);
			~ippp();

			void connect();
			void disconnect();

		private:
			network::socket_connection _con;
			packet::packet_builder _packet;
		};
	}
}
