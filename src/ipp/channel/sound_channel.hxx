#pragma once

#include <array>
#include "ipp/channel/channel_wrapper.hxx"

namespace ipp {
	class ipphone;
	namespace channel {
		class sound_channel : public channel_wrapper {
			static constexpr std::size_t buffer_packet = 16;
		public:
			using channel_type = protocol::channel::channel_type;

			sound_channel(ipphone&, uint32_t ch_id, channel_type ch_type);

			virtual void receive(const std::uint8_t* buf, const std::uint16_t len) override;
			virtual void flush_packets() override;
			virtual void close() override;

		protected:
			ipphone& _ipp;
		};
	}
}
