#pragma once

#include <chrono>
#include "ipp/network/socket_address.hxx"
#include "ipp/network/socket_connection.hxx"
#include "ipp/shared_fd.h"

namespace ipp {
	namespace network {
		class socket {
		public:
			using timer_type = std::chrono::microseconds;
			socket();

			socket_connection connect(const socket_address& addr);

			void bind(const socket_address&);

			void listen(int backlog);

			bool have_incoming_connection(const timer_type&);

			socket_connection accept();

		protected:
			shared_fd _fd;
		};
	}
}
