#pragma once

#include <cstdint>

namespace ipp {
	namespace protocol {
		namespace message {
			enum class channel_flag : std::uint8_t {
				none = 0,
				receivable = 1,
			};
		}
	}
}
