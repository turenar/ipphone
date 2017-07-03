#pragma once

#include <cstdint>

namespace ipp {
	namespace protocol {
		namespace channel {
			enum class channel_flag : std::uint8_t {
				none = 0,
				receive_only = 1,
			};
		}
	}
}
