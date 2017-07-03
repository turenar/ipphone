#include "ipp/network/socket_connection.hxx"
#include <boost/throw_exception.hpp>
#include "ipp/network/socket_exception.hxx"


namespace ipp {
	namespace network {
		namespace {
			void debug_recv_send(const char* ev, const std::uint8_t* data, std::size_t buflen, int fd) {
				(void) ev;
				(void) data;
				(void) buflen;
#ifndef NDEBUG
				std::ostringstream buf;
				buf << ev << ": fd=" << fd << std::hex;
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
					break;
				}
				LOG(DEBUG) << buf.str();
#endif
			}
		}

		socket_connection::socket_connection(shared_fd fd, socket_address addr) : _fd(std::move(fd)) {
			_addr = std::move(addr);
		}

		std::size_t socket_connection::send(const std::uint8_t* data, std::size_t len) {
			debug_recv_send("send", data, len, _fd);
			ssize_t result = ::send(_fd, data, len, 0);
			if (result < 0) {
				IPP_THROW_EXCEPTION(socket_exception(errno, std::system_category()));
			}
			return static_cast<std::size_t>(result);
		}

		std::size_t socket_connection::recv(std::uint8_t* buf, std::size_t buflen) {
			ssize_t result = ::recv(_fd, buf, buflen, 0);
			if (result < 0) {
				IPP_THROW_EXCEPTION(socket_exception(errno, std::system_category()));
			}
			debug_recv_send("recv", buf, result, _fd);
			return static_cast<std::size_t>(result);
		}

		bool socket_connection::recvable(const std::chrono::milliseconds& timeout) {
			fd_set rfds;
			FD_ZERO(&rfds);
			FD_SET(_fd.get(), &rfds);

			timeval tv;
			auto timeout_sec = std::chrono::duration_cast<std::chrono::seconds>(timeout);
			tv.tv_sec = timeout_sec.count();
			tv.tv_usec = (timeout - timeout_sec).count();
			int result = ::select(_fd.get() + 1, &rfds, NULL, NULL, &tv);
			if (result < 0) {
				IPP_THROW_EXCEPTION(socket_exception(errno, std::system_category()));
			}
			return result > 0;
		}

		socket_address& socket_connection::address() {
			return _addr;
		}
	}
}
