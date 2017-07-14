#pragma once

#include "ipp/ipphone.hxx"
#include "ipp/channel/channel_wrapper.hxx"

extern "C" {
#include <libavcodec/avcodec.h>
}

namespace ipp {
	namespace channel {
		class video_decoder_channel final : public channel_wrapper {
		public:
			using channel_type = protocol::channel::channel_type;
			using callback_type = std::function<void(uint8_t*, int, int, int)>;
			static constexpr std::size_t inbuf_size = 65536;

			video_decoder_channel(ipphone&, uint32_t ch_id, channel_type ch_type);

			virtual void receive(const std::uint8_t* buf, const std::uint16_t len) override;
			virtual void flush_packets() override;
			virtual void close() override;

			void callback(callback_type c);

		protected:
			ipphone& _ipp;
			callback_type _callback;

			const AVCodec* _codec;
			AVCodecContext* _context = NULL;
			int _frame_count = 0;
			AVFrame* _frame;
			uint8_t _inbuf[inbuf_size + AV_INPUT_BUFFER_PADDING_SIZE];
			AVPacket _avpkt;

			void callback(uint8_t* mono_data, int line_height, int width, int height);
		};
	}
}
