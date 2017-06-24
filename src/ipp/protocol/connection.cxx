#include "ipp/protocol/connection.hxx"

namespace ipp {
	namespace protocol {
		connection::connection(network::socket_connection&& prot) : _protocol(prot) {
		}

		void connection::parse_message(const std::uint8_t* msg, std::size_t len) {
		}

		ippp& connection::protocol() {
			return _protocol;
		}
	}
}
