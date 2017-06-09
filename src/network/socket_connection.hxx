#pragma once

#include <cstdint>
#include "misc.hxx"

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
			std::size_t recv(std::uint8_t* buf, std::size_t buflen);

		private:
			fd_type _fd;
		};
	}
}
