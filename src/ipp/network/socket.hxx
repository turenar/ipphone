#pragma once

#include "ipp/network/socket_address.hxx"
#include "ipp/network/socket_connection.hxx"
#include "ipp/shared_fd.h"

namespace ipp {
	namespace network {
		class socket {
		public:
			socket();

			socket_connection connect(const socket_address& addr);

		protected:
			shared_fd _fd;
		};
	}
}
