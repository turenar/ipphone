#include "ipp/protocol/connection_manager.hxx"
#include <cstring>
#include "ipp/logger/logger.hxx"
#include "ipp/protocol/message/message.hxx"

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
		}

		connection_manager::connection_manager(network::socket&& sock) : _sock(std::move(sock)) {
		}

		void connection_manager::consume_socket() {
			std::uint8_t buf[65536];
			network::socket_address addr;
			while (_sock.recvable(std::chrono::milliseconds(0))) {
				std::size_t len = _sock.recv(buf, sizeof(buf), addr);
				std::size_t message_len = parse_packet(buf, len);
				if (message_len <= 0) {
					continue; // broken packet
				}

				std::uint8_t* message = buf + sizeof(packet::packet_header);
				auto iterator = _connection_map.find(addr);
				if (iterator == _connection_map.end()) {
					if (!is_connection_packet(message, message_len)) {
						continue;
					}
					LOG(DEBUG) << "incoming connection!";
					iterator = _connection_map.emplace(addr, network::socket_connection(_sock, addr)).first;
					iterator->second.protocol().connect();
				}
				iterator->second.parse_message(message, message_len);
			}
		}
	}
}
