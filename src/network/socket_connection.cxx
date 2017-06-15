#include <sys/socket.h>
#include <boost/throw_exception.hpp>
#include "socket_connection.hxx"
#include "socket_exception.hxx"


namespace ipp {
	namespace network {
		socket_connection::socket_connection(fd_type fd) : _fd(fd) {}

		std::size_t socket_connection::send(const std::uint8_t* data, std::size_t len) {
			ssize_t w = ::send(_fd, data, len, 0);
			if (w < 0) {
				IPP_THROW_EXCEPTION(socket_exception(errno, std::system_category()));
			}
			return static_cast<std::size_t>(w);
		}

		std::size_t socket_connection::recv(std::uint8_t* buf, std::size_t buflen) {
			ssize_t w = ::recv(_fd, buf, buflen, 0);
			if (w < 0) {
				IPP_THROW_EXCEPTION(socket_exception(errno, std::system_category()));
			}
			return static_cast<std::size_t>(w);
		}
	}
}
