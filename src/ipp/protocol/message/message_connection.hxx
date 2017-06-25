#pragma once

#include <cstdint>
#include "ipp/protocol/message/message_type.hxx"

namespace ipp {
	namespace protocol {
		namespace message {
			struct keep_alive {
				static constexpr message_type type = message_type::keep_alive;
			};

			struct connect {
				char protocol[6];
				std::uint8_t version;
				std::uint8_t revision;
				static constexpr message_type type = message_type::connect;
			};

			struct disconnect {
				static constexpr message_type type = message_type::disconnect;
			};
		}
	}
}
