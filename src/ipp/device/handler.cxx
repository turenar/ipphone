#include "ipp/device/handler.hxx"

namespace ipp {
	namespace device {
		handler::handler(std::size_t buffer_size) : _buf(buffer_size) {
		}

		handler::buffer_t& handler::buffer() {
			return _buf;
		}
	}
}
