#pragma once

#include <unordered_map>
#include "ipp/network/socket.hxx"
#include "ipp/protocol/connection.hxx"

namespace ipp {
	namespace protocol {
		class protocol_listener;

		class connection_manager {
		public:
			using listener_type = protocol_listener;
			using connection_map_type = std::unordered_map<network::socket_address, connection>;
			connection_manager(network::socket&&, listener_type& listener);
			connection_manager(const connection_manager&) = delete;
			connection_manager(connection_manager&&) = default;

			connection_manager& operator=(const connection_manager&) = delete;
			connection_manager& operator=(connection_manager&&) = default;

			connection& connect(const network::socket_address&);
			void consume_socket();

			connection_map_type& get_connections();

			network::socket& get_socket();
		private:
			network::socket _sock;
			listener_type& _listener;
			connection_map_type _connection_map;
		};
	}
}
