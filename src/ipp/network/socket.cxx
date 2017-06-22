#include "ipp/network/socket.hxx"
#include <boost/throw_exception.hpp>
#include "ipp/network/socket_exception.hxx"

namespace ipp {
	namespace network {
		socket::socket() {
			_fd = ::socket(AF_INET, SOCK_STREAM, 0);
			if (_fd < 0) {
				IPP_THROW_EXCEPTION(socket_exception(errno, std::system_category()));
			}
		}

		socket_connection socket::connect(const socket_address& addr) {
			int result = ::connect(_fd, addr.get_native_address(), addr.get_native_size());
			if (result < 0) {
				IPP_THROW_EXCEPTION(socket_exception(errno, std::system_category()));
			}
			return socket_connection(_fd);
		}
	}
}
