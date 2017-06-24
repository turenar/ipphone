#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string>

namespace ipp {
	namespace network {
		class socket_address {
			friend struct std::hash<socket_address>;

		public:
			socket_address();
			socket_address(sockaddr_in&& other);

			socket_address& set_address(const char* address_str);
			socket_address& set_address_any();
			socket_address& set_port(uint16_t port);

			std::uint16_t get_family() const;
			std::uint32_t get_address() const;
			std::string get_address_str() const;
			std::uint16_t get_port() const;

			const sockaddr* get_native_address() const;
			sockaddr* get_native_address();
			socklen_t get_native_size() const;

		private:
			struct sockaddr_in _addr;
		};

		inline bool operator==(const socket_address& lhs, const socket_address& rhs) {
			return lhs.get_family() == rhs.get_family()
			       && lhs.get_address() == rhs.get_address()
			       && lhs.get_port() == rhs.get_port();
		}

		inline bool operator!=(const socket_address& lhs, const socket_address& rhs) {
			return !(lhs == rhs);
		}
	}
}

namespace std {
	template <>
	struct hash<ipp::network::socket_address> {
		using argument_type = ipp::network::socket_address;
		using result_type = std::size_t;

		result_type operator()(const argument_type& s) const {
			return std::hash<std::uint16_t>()(s._addr.sin_family)
			       ^ std::hash<std::uint32_t>()(s._addr.sin_addr.s_addr)
			       ^ std::hash<std::uint16_t>()(s._addr.sin_port);
		}
	};
}
