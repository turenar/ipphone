#pragma once

#include "ipp/network/socket_connection.hxx"
#include "ipp/protocol/packet/packet.hxx"

namespace ipp {
	namespace protocol {
		namespace packet {
			class packet_builder {
			public:
				packet_builder();
				packet_builder(const packet_builder&) = default;
				packet_builder(packet_builder&&) = default;
				packet_builder& operator=(const packet_builder&) = default;
				packet_builder& operator=(packet_builder&&) = default;

				template <typename T, std::enable_if_t<std::is_integral<T>::value, std::nullptr_t> = nullptr>
				packet_builder& write(T);
				template <typename T, std::enable_if_t<std::is_pod<T>::value, std::nullptr_t> = nullptr>
				packet_builder& write(const T&);
				packet_builder& write(const std::int8_t* data, std::size_t len);

				void send(network::socket_connection&);
				void clear();

			private:
				static constexpr std::size_t packet_header_size = sizeof(packet_header);

				std::uint8_t _buf[packet_max_size];
				std::uint16_t _read_ptr ;
				std::uint16_t _write_ptr;
			};

			template <typename T, std::enable_if_t<std::is_integral<T>::value, std::nullptr_t>>
			inline packet_builder& packet_builder::write(T val) {
				return this->write(reinterpret_cast<const std::int8_t*>(&val), sizeof(val));
			}

			template <typename T, std::enable_if_t<std::is_pod<T>::value, std::nullptr_t>>
			inline packet_builder& packet_builder::write(const T& obj) {
				return this->write(reinterpret_cast<const std::int8_t*>(&obj), sizeof(obj));
			}
		}
	}
}
