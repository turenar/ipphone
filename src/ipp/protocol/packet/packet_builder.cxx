#include "ipp/protocol/packet/packet_builder.hxx"
#include <cstring>
#include "ipp/protocol/protocol_exception.hxx"

namespace ipp {
	namespace protocol {
		namespace packet {
			packet_builder::packet_builder() {
				clear();
			}

			packet_builder& packet_builder::write(const std::uint8_t* data, std::size_t len) {
				if (_write_ptr + len > packet_max_size) {
					IPP_THROW_EXCEPTION(protocol_exception("packet size exceeded"));
				}
				std::memcpy(_buf + _write_ptr, data, len);
				_write_ptr += len;

				return *this;
			}

			void packet_builder::send(network::socket_connection& con) {
				std::uint16_t size = static_cast<uint16_t> (_write_ptr - packet_header_size);
				_buf[0] = size & 0xff;
				_buf[1] = size >> 8;
				_buf[2] = static_cast<std::uint8_t>(packet_flag::NONE);
				_buf[3] = 0;
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
