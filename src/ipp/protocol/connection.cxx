#include "ipp/protocol/connection.hxx"
#include "ipp/logger/logger.hxx"
#include "ipp/protocol/protocol_listener.hxx"
#include "ipp/protocol/message/message.hxx"

namespace ipp {
	namespace protocol {
		namespace {
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

		connection::connection(network::socket_connection&& con, protocol_listener& listener)
				: _protocol(con), _listener(listener) {
		}

		protocol_listener& connection::get_listener() {
			return _listener;
		}

		bool connection::parse_message(const std::uint8_t* msg, std::size_t len) {
			static_assert(sizeof(message::message_type) == 1, "message_type is too big");

			message::message_type type = static_cast<message::message_type>(*msg);
			switch (type) {
				case message::message_type::CONNECT:
					return dispatch<message::connect>(*this, msg, len, &listener_type::on_connect);
				case message::message_type::DISCONNECT:
					return dispatch<message::disconnect>(*this, msg, len, &listener_type::on_disconnect);
				default:
					LOG(WARNING) << "unknown message type: " << static_cast<std::uint8_t>(type);
					return false;
			}
		}

		ippp& connection::protocol() {
			return _protocol;
		}
	}
}
