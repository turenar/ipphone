#include "ipp/protocol/ippp.hxx"
#include "ipp/protocol/message/message.hxx"
#include "ipp/protocol/message/message_channel.hxx"
#include "ipp/protocol/message/message_connection.hxx"

namespace ipp {
	namespace protocol {
		ippp::ippp(network::socket_connection con) : _con(std::move(con)) {}

		ippp::~ippp() {}

		void ippp::keep_alive() {
			std::lock_guard <std::mutex> lock{_mutex};
			_packet.write(message::message_type::keep_alive)
					.write(message::keep_alive{})
					.send(_con);
		}

		void ippp::connect() {
			std::lock_guard <std::mutex> lock(_mutex);
			_packet.write(message::message_type::connect)
					.write(message::connect{IPPP_PROTOCOL_NAME, protocol_version, protocol_revision})
					.send(_con);
		}

		void ippp::disconnect() {
			std::lock_guard <std::mutex> lock(_mutex);
			_packet.write(message::message_type::disconnect)
					.write(message::disconnect{})
					.send(_con);
		}

		void ippp::channel_open(std::uint32_t ch_id, channel::channel_type ch_type, channel::channel_flag ch_flag) {
			std::lock_guard <std::mutex> lock(_mutex);
			_packet.write(message::message_type::channel_open)
					.write(message::channel_open{ch_id, ch_type, ch_flag, 0})
					.send(_con);
		}

		void ippp::channel_data(std::uint32_t ch_id, const std::uint8_t* data, std::uint16_t len) {
			std::lock_guard <std::mutex> lock(_mutex);
			_packet.write(message::message_type::channel_data)
					.write(message::channel_data{ch_id})
					.write(data, len)
					.send(_con);
		}

		void ippp::channel_close(std::uint32_t ch_id) {
			std::lock_guard <std::mutex> lock(_mutex);
			_packet.write(message::message_type::channel_close)
					.write(message::channel_close{ch_id})
					.send(_con);
		}
	}
}
