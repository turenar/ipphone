#include "ipp/network/socket_address.hxx"
#include <utility>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "ipp/ipp_exception.hxx"

namespace ipp {
	namespace network {
		socket_address::socket_address() {
			_addr.sin_family = AF_INET;
		}

		socket_address::socket_address(sockaddr_in&& other) {
			_addr = std::move(other);
		}

		socket_address& socket_address::set_address(const char* address_str) {
			int result = ::inet_aton(address_str, &this->_addr.sin_addr);
			if (!result) {
				IPP_THROW_EXCEPTION(ipp_exception(std::string(address_str) + " is not valid address"));
			}
			return *this;
		}

		socket_address& socket_address::set_port(uint16_t port) {
			this->_addr.sin_port = htons(port);
			return *this;
		}

		const sockaddr* socket_address::get_native_address() const {
			return reinterpret_cast<const sockaddr*>(&_addr);
		}

		sockaddr* socket_address::get_native_address() {
			return reinterpret_cast<sockaddr*>(&_addr);
		}

		socklen_t socket_address::get_native_size() const {
			return static_cast<socklen_t>(sizeof(_addr));
		}

		socket_address& socket_address::set_address_any() {
			this->_addr.sin_addr.s_addr = INADDR_ANY;
			return *this;
		}

		std::uint16_t socket_address::get_family() const {
			return this->_addr.sin_family;
		}

		std::uint32_t socket_address::get_address() const {
			return this->_addr.sin_addr.s_addr;
		}
		std::string socket_address::get_address_str() const {
			return ::inet_ntoa(this->_addr.sin_addr);
		}

		std::uint16_t socket_address::get_port() const {
			return ntohs(this->_addr.sin_port);
		}
	}
}
