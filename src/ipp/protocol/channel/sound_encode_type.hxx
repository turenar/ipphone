#pragma once

#include <cstdint>

namespace ipp {
	namespace protocol {
		namespace channel {
			enum class sound_encode_type : std::uint8_t {
				raw16, // 16bit/sample 1channel
			};
		}
	}
}
