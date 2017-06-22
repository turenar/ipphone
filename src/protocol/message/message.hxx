#pragma once

#include <cstdint>
#include <type_traits>
#include "protocol/message/message_type.hxx"

namespace ipp {
	namespace protocol {
		namespace message {
			template <typename MessageSpec>
			struct message_packer {
				message_type type;
				MessageSpec spec;
			};

			template <typename MessageSpec>
			constexpr message_packer<MessageSpec> pack(MessageSpec sp) {
				return message_packer<MessageSpec>{MessageSpec::type, sp};
			}

			struct connect {
				std::uint8_t version;
				std::uint8_t revision;
				static constexpr message_type type = message_type::CONNECT;
			};

			struct disconnect {
				static constexpr message_type type = message_type::DISCONNECT;
			};
		}
	}
}
