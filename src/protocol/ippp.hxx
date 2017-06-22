#pragma once

#include <memory>
#include "network/socket_connection.hxx"
#include "packet/packet.hxx"
#include "protocol/packet/packet_builder.hxx"

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
