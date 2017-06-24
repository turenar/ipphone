#include "ipp/network/socket.hxx"
#include <boost/throw_exception.hpp>
#include "ipp/network/socket_connection.hxx"
#include "ipp/network/socket_exception.hxx"

namespace ipp {
	namespace network {
		namespace {
			void debug_recv_send(const char* ev, const std::uint8_t* data, std::size_t buflen) {
				(void) ev;
				(void) data;
				(void) buflen;
#ifndef NDEBUG
				std::ostringstream buf;
				buf << ev << ": " << std::hex;
				const std::uint8_t* p = data;
				for (std::size_t i = 0; i < buflen; i += 16) {
					std::size_t colend = std::min(i + 16, buflen);
					buf << '\n' << std::setw(4) << i << ':';
					for (std::size_t col = i; col < colend; ++col) {
						if (col % 4 == 0) {
							buf << '|';
						} else {
							buf << ' ';
						}
						buf << std::setw(2) << std::setfill('0') << static_cast<int>(*p++);
					}
				}
				LOG(DEBUG) << buf.str();
#endif
			}
		}

		socket::socket() {
			_fd = shared_fd(::socket(AF_INET, SOCK_DGRAM, 0));
			if (!_fd) {
				IPP_THROW_EXCEPTION(socket_exception(errno, std::system_category()));
			}
		}

		socket_connection socket::connect(const socket_address& addr) {
			return socket_connection(*this, addr);
		}

		void socket::bind(const socket_address& addr) {
			int result = ::bind(_fd, addr.get_native_address(), addr.get_native_size());
			if (result < 0) {
				IPP_THROW_EXCEPTION(socket_exception(errno, std::system_category()));
			}
		}

		bool socket::recvable(const std::chrono::milliseconds& timeout) {
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

		std::size_t socket::send(const std::uint8_t* data, std::size_t len, const socket_address& addr) {
			debug_recv_send("send", data, len);
			ssize_t result = ::sendto(_fd, data, len, 0, addr.get_native_address(), addr.get_native_size());
			if (result < 0) {
				IPP_THROW_EXCEPTION(socket_exception(errno, std::system_category()));
			}
			return static_cast<std::size_t>(result);
		}

		std::size_t socket::recv(std::uint8_t* buf, std::size_t buflen, socket_address& addr) {
			socket_address tmp_addr;
			auto tmp_addr_len = tmp_addr.get_native_size();
			ssize_t result = ::recvfrom(_fd, buf, buflen, 0, tmp_addr.get_native_address(), &tmp_addr_len);
			if (result < 0) {
				IPP_THROW_EXCEPTION(socket_exception(errno, std::system_category()));
			}
			debug_recv_send("recv", buf, static_cast<std::size_t>(result));
			addr = std::move(tmp_addr);
			return static_cast<std::size_t>(result);
		}

		socket_address socket::get_listening_address() {
			socket_address addr;
			socklen_t addr_len = addr.get_native_size();
			::getsockname(_fd, addr.get_native_address(), &addr_len);
			return addr;
		}
	}
}
