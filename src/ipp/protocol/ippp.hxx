#pragma once

#include <memory>
#include "ipp/network/socket_connection.hxx"
#include "ipp/protocol/packet/packet_builder.hxx"
#include "packet/packet.hxx"

#define IPPP_PROTOCOL_NAME {'i', 'p', 'p', 'p', '\0', '\0'}
namespace ipp {
	namespace protocol {
		class ippp {
		public:
			static constexpr std::uint8_t protocol_version = 0;
			static constexpr std::uint8_t protocol_revision = 1;
			static constexpr char protocol_name[6] = IPPP_PROTOCOL_NAME;

			ippp(network::socket_connection);
			~ippp();

			void keep_alive();
			void connect();
			void disconnect();

		private:
			network::socket_connection _con;
			packet::packet_builder _packet;
		};
	}
}
