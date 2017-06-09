#pragma once

#include "socket_address.hxx"
#include "socket_connection.hxx"

namespace ipp {
	namespace network {
		class socket {
		public:
			socket();

			socket_connection connect(socket_address& addr);
		};
	}
}
