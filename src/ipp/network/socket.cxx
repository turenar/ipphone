#include "ipp/network/socket.hxx"
#include <boost/throw_exception.hpp>
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
			int result = ::bind(_fd, addr.get_native_address(), addr.get_native_size());
			if (result < 0) {
				IPP_THROW_EXCEPTION(socket_exception(errno, std::system_category()));
			}
		}


		void socket::listen(int backlog) {
			int result = ::listen(_fd, backlog);
			if (result < 0) {
				IPP_THROW_EXCEPTION(socket_exception(errno, std::system_category()));
			}
		}

		bool socket::have_incoming_connection(const timer_type& timeout) {
			fd_set read_set;
			FD_ZERO(&read_set);
			FD_SET(_fd.get(), &read_set);
			timeval tv;
			auto timeout_sec = std::chrono::duration_cast<std::chrono::seconds>(timeout);
			tv.tv_sec = timeout_sec.count();
			tv.tv_usec = (timeout - timeout_sec).count();
			int updated = ::select(1, &read_set, NULL, NULL, &tv);
			if (updated < 0) {
				IPP_THROW_EXCEPTION(socket_exception(errno, std::system_category()));
			}
			return updated > 0;
		}

		socket_connection socket::accept() {
			socket_address addr;
			socklen_t len = addr.get_native_size();
			int con_fd = ::accept(_fd, addr.get_native_address(), &len);
			if (con_fd < 0) {
				IPP_THROW_EXCEPTION(socket_exception(errno, std::system_category()));
			}
			return socket_connection(shared_fd(con_fd), addr);
		}
	}
}
