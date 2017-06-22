#pragma once

#include <cstdint>
#include "protocol/packet/packet_flag.hxx"


namespace ipp {
	namespace protocol {
		namespace packet {
			struct packet_header {
				std::uint16_t size;
				packet_flag flag;
				std::uint8_t sequence;
				std::uint8_t checksum;
				std::uint8_t reserved;
			};

			constexpr std::uint16_t packet_max_size = 60000u;
			constexpr std::uint16_t packet_max_body_size = packet_max_size - sizeof(packet_header);
		}
	}
}
