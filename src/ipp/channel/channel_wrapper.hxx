#pragma once

#include <cstdint>
#include <ipp/protocol/channel/channel_type.hxx>

namespace ipp {
	namespace channel {
		class channel_wrapper {
		public:
			using channel_type = protocol::channel::channel_type;

			channel_wrapper(std::uint32_t ch_id) : _channel_id(ch_id) {}

			virtual ~channel_wrapper() {}

			virtual channel_type type() const = 0;
			virtual std::uint32_t ch_id() const;

			virtual void receive(const std::uint8_t* buf, const std::uint16_t len) = 0;
			virtual void flush_packets() = 0;
			virtual void close() = 0;

		protected:
			std::uint32_t _channel_id;
		};

		inline std::uint32_t channel_wrapper::ch_id() const { return _channel_id; }
	}
}
