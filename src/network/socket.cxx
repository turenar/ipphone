#include "socket.hxx"

namespace ipp {
	namespace network {
		socket::socket(socket_address& addr) : _addr(addr) {
		}

		void socket::connect() {
			int fd = connect()
		}

		void send(uint8_t* data, size_t len) {

		}

		size_t recv(uint8_t* buf, size_t buflen) {}
	}
}
