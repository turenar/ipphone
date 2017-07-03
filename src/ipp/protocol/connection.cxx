#include "ipp/protocol/connection.hxx"
#include <cstring>
#include <limits>
#include <typeinfo>
#include "ipp/ipp_exception.hxx"
#include "ipp/logger/logger.hxx"
#include "ipp/protocol/protocol_listener.hxx"
#include "ipp/protocol/message/message_connection.hxx"

namespace ipp {
	namespace protocol {
		namespace {
			std::size_t parse_packet(std::uint8_t* buf, std::size_t len) {
				if (len < sizeof(packet::packet_header)) {
//					LOG(ERROR) << "corrupted packet; received len=" << len;
					return 0;
				}
				packet::packet_header* header = reinterpret_cast<packet::packet_header*>(buf);
				if (len < sizeof(packet::packet_header) + header->size) {
//					LOG(ERROR) << "corrupted packet; received len=" << len << ", packet len=" << header->size;
					return 0;
				}
				return header->size;
			}

			template <typename MessageType, typename Dispatcher>
			inline bool dispatch(connection& con, const std::uint8_t* msg, std::size_t len, Dispatcher dispatcher) {
				std::size_t required_len = std::is_empty<MessageType>::value ? 0 : sizeof(MessageType);
				if (len < required_len) {
					LOG(WARNING) << "broken message: required_size=" << required_len << ", actual_size=" << len;
					return false;
				}
//				LOG(INFO) << "dispatching " << typeid(Dispatcher).name();
				((&con.get_listener())->*dispatcher)(con, reinterpret_cast<const MessageType*>(msg + 1), len - 1);
				return true;
			}
		}

		connection::connection(network::socket_connection con, protocol_listener& listener)
				: _protocol(con), _con(con), _listener(listener) {
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
			while (_con.recvable(std::chrono::milliseconds(0))) {
				std::size_t len = _con.recv(_buf + _remain_len, sizeof(_buf) - _remain_len);
				_remain_len += len;
				if (len == 0) {
					IPP_THROW_EXCEPTION(ipp_exception("Connection closed"));
				}
				std::uint8_t* ptr = _buf;
				while (_remain_len > 0) {
					std::size_t message_len = parse_packet(ptr, _remain_len);
					if (message_len <= 0) {
						break; // broken packet
					}

					ptr += sizeof(packet::packet_header);
					parse_message(ptr, message_len);
					ptr += message_len;
					if (_remain_len > std::numeric_limits<std::int64_t>::max()) {
						LOG(ERROR) << _remain_len << ':' << len << ':' << message_len;
					}
					_remain_len -= sizeof(packet::packet_header) + message_len;
				}
				std::memmove(_buf, ptr, _remain_len);
			}
		}

		ippp& connection::protocol() {
			return _protocol;
		}
	}
}
