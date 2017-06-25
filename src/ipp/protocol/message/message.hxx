#pragma once

#include <cstdint>
#include <type_traits>
#include "ipp/protocol/message/message_type.hxx"

namespace ipp {
	namespace protocol {
		namespace message {
			template <typename MessageSpec, typename Enable>
			struct message_packer;

			template <typename MessageSpec>
			struct message_packer<MessageSpec, std::true_type> {
				message_type type;
			};
			template <typename MessageSpec>
			struct message_packer<MessageSpec, std::false_type> {
				message_type type;
				MessageSpec spec;
			};

			template <typename MessageSpec,
					std::enable_if_t<std::is_empty<MessageSpec>::value, std::nullptr_t> = nullptr>
			constexpr message_packer<MessageSpec, std::true_type> pack(MessageSpec&& sp) {
				(void) sp;
				return message_packer<MessageSpec, std::true_type>{MessageSpec::type};
			}

			template <typename MessageSpec,
					std::enable_if_t<!std::is_empty<MessageSpec>::value, std::nullptr_t> = nullptr>
			constexpr message_packer<MessageSpec, std::false_type> pack(MessageSpec sp) {
				return message_packer<MessageSpec, std::false_type>{MessageSpec::type, sp};
			}

			struct keep_alive {
				static constexpr message_type type = message_type::KEEP_ALIVE;
			};

			struct connect {
				char protocol[6];
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
