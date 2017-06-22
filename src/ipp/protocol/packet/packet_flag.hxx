#pragma once

#include <cstdint>
#include <type_traits>

namespace ipp {
	namespace protocol {
		namespace packet {
			enum class packet_flag : std::uint8_t {
				NONE = 0,
			};

			constexpr packet_flag operator|(packet_flag lhs, packet_flag rhs) {
				using flag_t = std::underlying_type_t<packet_flag>;
				return static_cast<packet_flag>(
						static_cast<flag_t>(lhs) | static_cast<flag_t>(rhs));
			}
		}
	}
}
