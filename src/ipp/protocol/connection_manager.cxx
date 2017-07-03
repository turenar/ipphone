#include "ipp/protocol/connection_manager.hxx"
#include <cstring>
#include "ipp/logger/logger.hxx"
#include "ipp/protocol/message/message.hxx"
#include "ipp/protocol/message/message_connection.hxx"

namespace ipp {
	namespace protocol {
		namespace {
			constexpr struct {
				message::message_type ty;
				message::connect c;
			} expected_connected = {message::message_type::connect,
			                        {IPPP_PROTOCOL_NAME, ippp::protocol_version, ippp::protocol_revision}};

			bool is_connection_packet(std::uint8_t* message, std::size_t len) {
				return len >= sizeof(expected_connected)
				       && 0 == ::memcmp(message, &expected_connected, sizeof(expected_connected));
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
		}

		connection_manager::connection_manager(network::socket&& sock, listener_type& listener)
				: _sock(std::move(sock)), _listener(listener) {
		}

		const std::unique_ptr<connection>& connection_manager::connect(const network::socket_address& addr) {
			auto iterator = _connection_map.find(addr);
			if (iterator == _connection_map.end()) {
				auto new_connection = std::make_unique<connection>(_sock.connect(addr), _listener);
				_connection_map.emplace(addr, std::move(new_connection));
				iterator = _connection_map.find(addr);
				iterator->second->protocol().connect();
			}
			return iterator->second;
		}

		void connection_manager::consume_socket() {
			if (_sock.listening() && _sock.accpetable(std::chrono::milliseconds::zero())) {
				network::socket_connection sock_con = _sock.accept();
				network::socket_address address = sock_con.address();
				_connection_map.emplace(std::move(address),
				                        std::make_unique<connection>(std::move(sock_con), _listener));
			}

			for (auto& con : _connection_map) {
				con.second->consume();
			}
		}

		connection_manager::connection_map_type& connection_manager::get_connections() {
			return _connection_map;
		}

		network::socket& connection_manager::get_socket() {
			return _sock;
		}
	}
}
