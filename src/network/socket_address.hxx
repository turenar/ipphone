#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace ipp {
	namespace network {
		class socket_address {
		public:
			socket_address() {
				_addr.sin_family = AF_INET;
			}

			socket_address& set_address(const char* address_str) {
				this->_addr.sin_addr.s_addr = inet_addr(address_str);
			}

			socket_address& set_port(uint16_t port) {
				this->_addr.sin_port = htons(port);
			}

			sockaddr* get_native_address() {
				return static_cast<sockaddr*>(&_addr);
			}

			int get_size() {
				return static_cast<int>(sizeof(_addr));
			}

		private:
			struct sockaddr_in _addr;
		};
	}
}
