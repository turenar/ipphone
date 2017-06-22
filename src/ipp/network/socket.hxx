#pragma once

#include "ipp/network/socket_address.hxx"
#include "ipp/network/socket_connection.hxx"

namespace ipp {
	namespace network {
		class socket {
		public:
			socket();

			socket_connection connect(const socket_address& addr);

		protected:
			fd_type _fd;
		};
	}
}
