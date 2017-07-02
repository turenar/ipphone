#pragma once

#include <cstdint>
#include "ipp/protocol/channel/sound_encode_type.hxx"

namespace ipp {
	namespace protocol {
		namespace channel {
			struct sound_header {
				sound_encode_type type;
				std::uint8_t reserved;
				std::uint16_t samples;
			};
		}
	}
}
