#include "ipp/protocol/ippp.hxx"
#include "ipp/protocol/message/message.hxx"
#include "ipp/protocol/message/message_channel.hxx"
#include "ipp/protocol/message/message_connection.hxx"

namespace ipp {
	namespace protocol {
		ippp::ippp(network::socket_connection con) : _con(std::move(con)) {}

		ippp::~ippp() {}

		void ippp::keep_alive() {
			_packet.write(message::pack(message::keep_alive{}))
					.send(_con);
		}
		void ippp::connect() {
			_packet.write(message::pack(message::connect{IPPP_PROTOCOL_NAME, protocol_version, protocol_revision}))
					.send(_con);
		}

		void ippp::disconnect() {
			_packet.write(message::pack(message::disconnect{}))
					.send(_con);
		}

		void ippp::channel_open(std::uint8_t ch_id, message::channel_type ch_type, message::channel_flag ch_flag) {
			_packet.write(message::pack(message::channel_open{ch_id, ch_type, ch_flag, 0}))
					.send(_con);
		}

		void ippp::channel_data(std::uint8_t ch_id, const std::uint8_t* data, std::uint16_t len) {
			_packet.write(message::pack(message::channel_data{ch_id}))
					.write(data, len)
					.send(_con);
		}

		void ippp::channel_close(std::uint8_t ch_id) {
			_packet.write(message::pack(message::channel_close{ch_id}))
					.send(_con);
		}
	}
}
