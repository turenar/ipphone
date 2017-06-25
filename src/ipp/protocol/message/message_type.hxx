#pragma once

#include <cstdint>

namespace ipp {
	namespace protocol {
		namespace message {
			enum class message_type : std::uint8_t {
				keep_alive,
				connect,
				disconnect,
				channel_open,
				channel_data,
				channel_close,
			};
		}
	}
}
