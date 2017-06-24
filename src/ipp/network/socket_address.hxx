#pragma once

#include <utility>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string>

namespace ipp {
	namespace network {
		class socket_address {
		public:
			socket_address();
			socket_address(sockaddr_in&& other);

			socket_address& set_address(const char* address_str);
			socket_address& set_address_any();
			socket_address& set_port(uint16_t port);

			std::string get_address_str() const;
			std::uint16_t get_port() const;

			const sockaddr* get_native_address() const;
			sockaddr* get_native_address();
			socklen_t get_native_size() const;

		private:
			struct sockaddr_in _addr;
		};
	}
}
