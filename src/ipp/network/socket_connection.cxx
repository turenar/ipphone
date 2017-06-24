#include "ipp/network/socket_connection.hxx"
#include <boost/throw_exception.hpp>
#include <sys/socket.h>
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

		socket_connection::socket_connection() {}

		socket_connection::socket_connection(shared_fd fd, socket_address addr) {
			_fd = std::move(fd);
			_addr = std::move(addr);
		}

		bool socket_connection::valid() const {
			return static_cast<bool>(_fd);
		}

		socket_connection::operator bool() const {
			return valid();
		}

		std::size_t socket_connection::send(const std::uint8_t* data, std::size_t len) {
			debug_recv_send("send", data, len);
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
			debug_recv_send("recv", buf, static_cast<std::size_t>(w));
			return static_cast<std::size_t>(w);
		}
	}
}
