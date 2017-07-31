#pragma once

#include <sixel.h>
#include <memory>
#include "ipp/channel/video_decoder_channel.hxx"

extern "C" {
#include "libswscale/swscale.h"
}

namespace ippbin {
	class sixel_animation {
	public:
		sixel_animation() = default;
		sixel_animation(const sixel_animation&) = delete;
		sixel_animation(sixel_animation&&) = delete;
		sixel_animation& operator=(const sixel_animation&) = delete;
		sixel_animation& operator=(sixel_animation&&) = delete;
		~sixel_animation();

		void data(const AVFrame*, int width, int height);

	private:
		sixel_allocator_t* _allocator = nullptr;
		sixel_encoder_t* _encoder = nullptr;
		sixel_dither_t* _dither = nullptr;
		sixel_output_t* _output = nullptr;
		SwsContext* _sws_context = nullptr;
		std::uint8_t* _buffer[4];
		int _buffer_linesize[4];

		void initialize(int width, int height, AVPixelFormat format);
	};
}
