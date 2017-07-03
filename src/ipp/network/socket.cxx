#include "ipp/network/socket.hxx"
#include <boost/throw_exception.hpp>
#include "ipp/network/socket_connection.hxx"
#include "ipp/network/socket_exception.hxx"

namespace ipp {
	namespace network {
		socket::socket() {
			_fd = shared_fd(::socket(AF_INET, SOCK_STREAM, 0));
			if (!_fd) {
				IPP_THROW_EXCEPTION(socket_exception(errno, std::system_category()));
			}
		}

		socket_connection socket::connect(const socket_address& addr) {
			int result = ::connect(_fd, addr.get_native_address(), addr.get_native_size());
			if (result < 0) {
				IPP_THROW_EXCEPTION(socket_exception(errno, std::system_category()));
			}
			return socket_connection(_fd, addr);
		}

		void socket::bind(const socket_address& addr) {
			int on = 1;
			int result = setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
			if (result < 0) {
				IPP_THROW_EXCEPTION(socket_exception(errno, std::system_category()));
			}

			result = ::bind(_fd, addr.get_native_address(), addr.get_native_size());
			if (result < 0) {
				IPP_THROW_EXCEPTION(socket_exception(errno, std::system_category()));
			}

			result = ::listen(_fd, 1024);
			if (result < 0) {
				IPP_THROW_EXCEPTION(socket_exception(errno, std::system_category()));
			}
			_listening = true;
		}

		bool socket::accpetable(const std::chrono::milliseconds& timeout) {
			fd_set rfds;
			FD_ZERO(&rfds);
			FD_SET(_fd, &rfds);

			timeval tv;
			auto timeout_sec = std::chrono::duration_cast<std::chrono::seconds>(timeout);
			tv.tv_sec = timeout_sec.count();
			tv.tv_usec = (timeout - timeout_sec).count();
			int result = ::select(_fd + 1, &rfds, NULL, NULL, &tv);
			if (result < 0) {
				IPP_THROW_EXCEPTION(socket_exception(errno, std::system_category()));
			}
			return result > 0;
		}

		socket_connection socket::accept() {
			socket_address addr;
			socklen_t addr_len = addr.get_native_size();
			int con_fd = ::accept(_fd, addr.get_native_address(), &addr_len);
			if (con_fd < 0) {
				IPP_THROW_EXCEPTION(socket_exception(errno, std::system_category()));
			}
			return socket_connection(shared_fd(con_fd), std::move(addr));
		}

		socket_address socket::get_listening_address() {
			socket_address addr;
			socklen_t addr_len = addr.get_native_size();
			::getsockname(_fd, addr.get_native_address(), &addr_len);
			return addr;
		}

		bool socket::listening() {
			return _listening;
		}
	}
}
