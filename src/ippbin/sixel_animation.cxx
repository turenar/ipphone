#include "ippbin/sixel_animation.hxx"
#include "ipp/logger/logger.hxx"
#include "ipp/ipp_exception.hxx"
#include <cstring>

extern "C" {
#include <libavutil/imgutils.h>
}

namespace ippbin {
	namespace {
		int sixel_write(char* data, int size, void* priv) {
			return static_cast<int>(fwrite(data, 1, static_cast<std::size_t>(size), static_cast<FILE*>(priv)));
		}

		struct sixel_exception : public ipp::ipp_exception {
			sixel_exception(const std::string& s) : ipp_exception(s) {}
		};
	}

	sixel_animation::~sixel_animation() {
		if (_encoder) {
			sixel_encoder_unref(_encoder);
		}
		if (_output) {
			sixel_output_unref(_output);
		}
		if (_allocator) {
			sixel_allocator_unref(_allocator);
		}
		if (_dither) {
			sixel_dither_unref(_dither);
		}
		if (_buffer[0]) {
			av_freep(&_buffer[0]);
		}
		if (_sws_context) {
			sws_freeContext(_sws_context);
		}
	}

	void sixel_animation::initialize(int w, int h, AVPixelFormat format) {
		if (_frame_number >= 0) {
			return;
		}
		_frame_number = 0;
		int result = sixel_allocator_new(&_allocator, &malloc, &calloc, &realloc, &free);
		if (SIXEL_FAILED(result)) {
			IPP_THROW_EXCEPTION(sixel_exception(sixel_helper_format_error(result)));
		}
		result = sixel_output_new(&_output, &sixel_write, stderr, _allocator);
		if (SIXEL_FAILED(result)) {
			IPP_THROW_EXCEPTION(sixel_exception(sixel_helper_format_error(result)));
		}
		result = sixel_encoder_new(&_encoder, nullptr);
		if (SIXEL_FAILED(result)) {
			IPP_THROW_EXCEPTION(sixel_exception(sixel_helper_format_error(result)));
		}

		// align==1 can pass directly raw data to libsixel!
		int ret = av_image_alloc(_buffer, _buffer_linesize, w, h, AV_PIX_FMT_RGB24, 1);
		if (ret < 0) {
			IPP_THROW_EXCEPTION(sixel_exception("av_image_alloc failed"));
		}
		_sws_context = sws_getContext(w, h, format,
		                              w, h, AV_PIX_FMT_RGB24,
		                              SWS_BILINEAR, nullptr, nullptr, nullptr);
		if (!_sws_context) {
			IPP_THROW_EXCEPTION(sixel_exception("sws_getContext failed"));
		}
	}

	void sixel_animation::data(const AVFrame* fr, int width, int height) {
		initialize(width, height, AVPixelFormat(fr->format));
		int result;
		sws_scale(_sws_context, static_cast<const std::uint8_t* const*>(fr->data), fr->linesize, 0, height,
		          _buffer, _buffer_linesize);
		if ((_frame_number & 0xf) == 0) {
			if (_dither) {
				sixel_dither_unref(_dither);
			}
			result = sixel_dither_new(&_dither, 256, _allocator);
			if (SIXEL_FAILED(result)) {
				IPP_THROW_EXCEPTION(sixel_exception(sixel_helper_format_error(result)));
			}
			result = sixel_dither_initialize(_dither, _buffer[0], width, height,
			                                 SIXEL_PIXELFORMAT_RGB888, LARGE_AUTO, REP_AUTO, QUALITY_AUTO);
			if (SIXEL_FAILED(result)) {
				IPP_THROW_EXCEPTION(sixel_exception(sixel_helper_format_error(result)));
			}
		}
		result = sixel_encode(_buffer[0], width, height, 8, _dither, _output);
		if (SIXEL_FAILED(result)) {
			IPP_THROW_EXCEPTION(sixel_exception(sixel_helper_format_error(result)));
		}
	}
}
