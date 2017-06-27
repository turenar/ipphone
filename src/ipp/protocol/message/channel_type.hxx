#pragma once

#include <cstdint>

namespace ipp {
	namespace protocol {
		namespace message {
			enum class channel_type : std::uint8_t {
				unknown,
				sound_mono,
				sound_stereo,
			};
		}
	}
}
