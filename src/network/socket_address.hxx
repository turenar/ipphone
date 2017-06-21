#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

namespace ipp {
	namespace network {
		class socket_address {
		public:
			socket_address() {
				_addr.sin_family = AF_INET;
			}

			socket_address& set_address(const char* address_str) {
				this->_addr.sin_addr.s_addr = inet_addr(address_str);
				return *this;
			}

			socket_address& set_port(uint16_t port) {
				this->_addr.sin_port = htons(port);
				return *this;
			}

			const sockaddr* get_native_address() const {
				return reinterpret_cast<const sockaddr*>(&_addr);
			}

			int get_native_size() const {
				return static_cast<int>(sizeof(_addr));
			}

		private:
			struct sockaddr_in _addr;
		};
	}
}
