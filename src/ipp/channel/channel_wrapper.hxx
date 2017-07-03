#pragma once

#include <cstdint>
#include <ipp/protocol/channel/channel_type.hxx>

namespace ipp {
	namespace channel {
		class channel_wrapper {
		public:
			using channel_type = protocol::channel::channel_type;

			channel_wrapper(std::uint32_t ch_id, channel_type ch_type)
					: _channel_id(ch_id), _channel_type(ch_type) {}

			virtual ~channel_wrapper() {}

			virtual channel_type type() const;
			virtual std::uint32_t ch_id() const;
			virtual void ch_id(std::uint32_t);

			virtual void receive(const std::uint8_t* buf, const std::uint16_t len) = 0;
			virtual void flush_packets() = 0;
			virtual void close() = 0;

		protected:
			std::uint32_t _channel_id;
			channel_type _channel_type;
		};

		inline std::uint32_t channel_wrapper::ch_id() const { return _channel_id; }

		inline void channel_wrapper::ch_id(std::uint32_t id) { _channel_id = id; }

		inline channel_wrapper::channel_type channel_wrapper::type() const {
			return _channel_type;
		}
	}
}
