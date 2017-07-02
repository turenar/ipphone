#include "ipp/protocol/connection.hxx"
#include <cstring>
#include "ipp/logger/logger.hxx"
#include "ipp/protocol/protocol_listener.hxx"
#include "ipp/protocol/message/message_connection.hxx"

namespace ipp {
	namespace protocol {
		namespace {
			bool is_connection_packet(std::uint8_t* message, std::size_t len) {
				auto expected = message::pack(
						message::connect{IPPP_PROTOCOL_NAME, ippp::protocol_version, ippp::protocol_revision});
				return len >= sizeof(expected)
				       && 0 == ::memcmp(message, &expected, sizeof(expected));
			}

			std::size_t parse_packet(std::uint8_t* buf, std::size_t len) {
				if (len < sizeof(packet::packet_header)) {
					LOG(ERROR) << "corrupted packet; received len=" << len;
					return 0;
				}
				packet::packet_header* header = reinterpret_cast<packet::packet_header*>(buf);
				if (len < header->size) {
					LOG(ERROR) << "corrupted packet; received len=" << len << ", packet len=" << header->size;
					return 0;
				}
				return header->size;
			}

			template <typename MessageType, typename Dispatcher>
			inline bool dispatch(connection& con, const std::uint8_t* msg, std::size_t len, Dispatcher dispatcher) {
				using is_empty_body = std::integral_constant<bool, std::is_empty<MessageType>::value>;
				std::size_t required_len = sizeof(message::message_packer<MessageType, is_empty_body>);
				if (len < required_len) {
					LOG(WARNING) << "broken message: required_size=" << required_len << ", actual_size=" << len;
					return false;
				}
				((&con.get_listener())->*dispatcher)(con, reinterpret_cast<const MessageType*>(msg + 1), len - 1);
				return true;
			}
		}

		connection::connection(network::socket_connection con, protocol_listener& listener)
				: _con(con), _protocol(con), _buf(_listener(listener) {
		}

		protocol_listener& connection::get_listener() {
			return _listener;
		}

		bool connection::parse_message(const std::uint8_t* msg, std::size_t len) {
			static_assert(sizeof(message::message_type) == 1, "message_type is too big");

			message::message_type type = static_cast<message::message_type>(*msg);
			switch (type) {
				case message::message_type::connect:
					return dispatch<message::connect>(*this, msg, len, &listener_type::on_connect);
				case message::message_type::disconnect:
					return dispatch<message::disconnect>(*this, msg, len, &listener_type::on_disconnect);
				case message::message_type::keep_alive:
					return dispatch<message::keep_alive>(*this, msg, len, &listener_type::on_keep_alive);
				case message::message_type::channel_open:
					return dispatch<message::channel_open>(*this, msg, len, &listener_type::on_channel_open);
				case message::message_type::channel_data:
					return dispatch<message::channel_data>(*this, msg, len, &listener_type::on_channel_data);
				case message::message_type::channel_close:
					return dispatch<message::channel_close>(*this, msg, len, &listener_type::on_channel_close);
				default:
					LOG(WARNING) << "unknown message type: " << static_cast<std::uint8_t>(type);
					return false;
			}
		}

		void connection::consume() {
			std::uint8_t buf[65536];
			while (_con.recvable(std::chrono::milliseconds(0))) {
				std::size_t len = _con.recv(buf, sizeof(buf));
				if (len == 0) {
					break;
				}
				std::size_t message_len = parse_packet(buf, len);
				if (message_len <= 0) {
					continue; // broken packet
				}

				std::uint8_t* message = buf + sizeof(packet::packet_header);
				parse_message(message, message_len);
			}
		}

		ippp& connection::protocol() {
			return _protocol;
		}
	}
}
