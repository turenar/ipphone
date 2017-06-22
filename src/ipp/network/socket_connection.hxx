#pragma once

#include <cstdint>
#include <type_traits>
#include "ipp/misc.hxx"

namespace ipp {
	namespace network {
		class socket_connection {
		public:
			explicit socket_connection(fd_type fd);
			socket_connection(const socket_connection&) = delete;
			socket_connection(socket_connection&&) = default;

			socket_connection& operator=(const socket_connection&) = delete;
			socket_connection& operator=(socket_connection&&) = default;

			std::size_t send(const std::uint8_t* data, std::size_t len);
			template <typename T, std::enable_if_t<std::is_pod<T>::value, std::nullptr_t> = nullptr>
			std::size_t send(const T&);
			std::size_t recv(std::uint8_t* buf, std::size_t buflen);

		private:
			fd_type _fd;
		};

		template <typename T, std::enable_if_t<std::is_pod<T>::value, std::nullptr_t>>
		inline std::size_t socket_connection::send(const T& obj) {
			return this->send(reinterpret_cast<const std::uint8_t*>(&obj), sizeof(T));
		}
	}
}
