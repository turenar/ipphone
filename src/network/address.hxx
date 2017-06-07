#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace network {
	class socket_address {
	public:
		socket_address() {
			addr.sin_family = AF_INET;
		}

		socket_address& set_address(const char* address_str) {
			this->addr.sin_addr.s_addr = inet_addr(address_str);
		}

		socket_address& set_port(uint16_t port) {
			this->addr.sin_port = htons(port);
		}

		sockaddr* get_native_address() {
			return static_cast<sockaddr*>(&addr);
		}

	private:
		struct sockaddr_in addr;
	};
}
