#include "ipp/network/socket_connection.hxx"
#include <boost/throw_exception.hpp>
#include "ipp/network/socket_exception.hxx"


namespace ipp {
	namespace network {
		socket_connection::socket_connection(socket& sock, socket_address addr) : _socket(sock) {
			_addr = std::move(addr);
		}

		std::size_t socket_connection::send(const std::uint8_t* data, std::size_t len) {
			return _socket.send(data, len, _addr);
		}
	}
}
