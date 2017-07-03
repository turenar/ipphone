#pragma once

#include <memory>
#include <mutex>
#include "ipp/network/socket_connection.hxx"
#include "ipp/protocol/channel/channel_flag.hxx"
#include "ipp/protocol/channel/channel_type.hxx"
#include "ipp/protocol/packet/packet.hxx"
#include "ipp/protocol/packet/packet_builder.hxx"

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

			void channel_open(std::uint32_t ch_id, channel::channel_type ch_type, channel::channel_flag ch_flag);
			void channel_data(std::uint32_t ch_id, const std::uint8_t* data, std::uint16_t len);
			void channel_close(std::uint32_t ch_id);

		private:
			network::socket_connection _con;
			packet::packet_builder _packet;
			std::mutex _mutex;
		};
	}
}
