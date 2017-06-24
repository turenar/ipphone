#pragma once

#include <chrono>
#include "ipp/network/socket_address.hxx"
#include "ipp/shared_fd.h"

namespace ipp {
	namespace network {
		class socket_connection;

		class socket {
		public:
			using timer_type = std::chrono::microseconds;
			socket();

			socket_connection connect(const socket_address& addr);
			void bind(const socket_address&);

			std::size_t send(const std::uint8_t* data, std::size_t len, const socket_address& addr);
			std::size_t recv(std::uint8_t* buf, std::size_t buflen, socket_address& addr);

			socket_address get_listening_address();
		protected:
			shared_fd _fd;
		};
	}
}
