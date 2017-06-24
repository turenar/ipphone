#pragma once

#include <unordered_map>
#include "ipp/network/socket.hxx"
#include "ipp/protocol/connection.hxx"

namespace ipp {
	namespace protocol {
		class connection_manager {
		public:
			connection_manager(network::socket&&);
			connection_manager(const connection_manager&) = delete;
			connection_manager(connection_manager&&) = default;

			connection_manager& operator=(const connection_manager&) = delete;
			connection_manager& operator=(connection_manager&&) = default;

			void consume_socket();

		private:
			network::socket _sock;
			std::unordered_map<network::socket_address, connection> _connection_map;
		};
	}
}
