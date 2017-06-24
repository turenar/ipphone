#pragma once

#include <cstdint>
#include <type_traits>
#include "ipp/shared_fd.h"
#include "ipp/network/socket.hxx"

namespace ipp {
	namespace network {
		class socket_connection {
		public:
			socket_connection(socket&, socket_address);
			socket_connection(const socket_connection&) = default;
			socket_connection(socket_connection&&) = default;

			socket_connection& operator=(const socket_connection&) = default;
			socket_connection& operator=(socket_connection&&) = default;

			std::size_t send(const std::uint8_t* data, std::size_t len);
			template <typename T, std::enable_if_t<std::is_pod<T>::value, std::nullptr_t> = nullptr>
			std::size_t send(const T&);

		private:
			socket& _socket;
			socket_address _addr;
		};

		template <typename T, std::enable_if_t<std::is_pod<T>::value, std::nullptr_t>>
		inline std::size_t socket_connection::send(const T& obj) {
			return this->send(reinterpret_cast<const std::uint8_t*>(&obj), sizeof(T));
		}
	}
}
