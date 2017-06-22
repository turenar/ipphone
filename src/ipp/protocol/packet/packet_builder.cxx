#include "protocol/packet/packet_builder.hxx"
#include <cstring>
#include "protocol/protocol_exception.hxx"

namespace ipp {
	namespace protocol {
		namespace packet {
			packet_builder::packet_builder() {
				clear();
			}

			packet_builder& packet_builder::write(const std::int8_t* data, std::size_t len) {
				if (_write_ptr + len > packet_max_size) {
					throw protocol_exception("packet size exceeded");
				}
				std::memcpy(_buf + _write_ptr, data, len);
				_write_ptr += len;

				return *this;
			}

			void packet_builder::send(network::socket_connection& con) {
				con.send(_buf, _write_ptr);
				clear();
			}

			void packet_builder::clear() {
				_read_ptr = packet_header_size;
				_write_ptr = packet_header_size;
			}
		}
	}
}
