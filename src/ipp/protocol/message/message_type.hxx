#pragma once

#include <cstdint>

namespace ipp {
	namespace protocol {
		namespace message {
			enum class message_type : std::uint8_t {
				KEEP_ALIVE,
				CONNECT,
				DISCONNECT,
			};
		}
	}
}
