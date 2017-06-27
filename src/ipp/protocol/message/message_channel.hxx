#pragma once

#include <cstdint>
#include "ipp/protocol/message/channel_flag.hxx"
#include "ipp/protocol/message/channel_type.hxx"
#include "ipp/protocol/message/message.hxx"
#include "ipp/protocol/message/message_type.hxx"

namespace ipp {
	namespace protocol {
		namespace message {
			struct channel_open {
				std::uint8_t ch_id;
				channel_type ch_type;
				channel_flag ch_flag;
				std::uint8_t ch_reserved;

				static constexpr message_type type = message_type::channel_open;
			};

			struct channel_data {
				std::uint8_t ch_id;
				// data is following...
				static constexpr message_type type = message_type::channel_data;
			};

			struct channel_close {
				std::uint8_t ch_id;
				static constexpr message_type type = message_type::channel_close;
			};
		}
	}
}
