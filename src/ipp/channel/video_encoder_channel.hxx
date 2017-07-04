#pragma once

#include "ipp/ipphone.hxx"
#include "ipp/channel/channel_wrapper.hxx"

namespace ipp {
	namespace channel {
		class video_encoder_channel : public channel_wrapper {
		public:
			using channel_type = protocol::channel::channel_type;

			video_encoder_channel(ipphone&, uint32_t ch_id, channel_type ch_type);

			virtual void receive(const std::uint8_t* buf, const std::uint16_t len) override;
			virtual void flush_packets() override;
			virtual void close() override;

		protected:
			ipphone& _ipp;
			unique_fd _read_fd;
		};
	}
}
