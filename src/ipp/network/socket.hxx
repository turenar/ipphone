#pragma once

#include <chrono>
#include "ipp/shared_fd.h"
#include "ipp/network/socket_address.hxx"

namespace ipp {
	namespace network {
		class socket_connection;

		class socket {
		public:
			using timer_type = std::chrono::microseconds;
			socket();

			socket_connection connect(const socket_address& addr);
			void bind(const socket_address&);

			bool accpetable(const std::chrono::milliseconds& timeout);
			socket_connection accept();

			socket_address get_listening_address();
			bool listening();
		protected:
			shared_fd _fd;
			bool _listening = false;
		};
	}
}
