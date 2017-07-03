#pragma once

#include <cstdint>

namespace ipp {
	namespace protocol {
		namespace channel {
			enum class channel_type : std::uint8_t {
				unknown,
				sound,
				file,
			};
		}
	}
}
