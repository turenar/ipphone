#include "ippbin/sixel_animation.hxx"
#include "ipp/logger/logger.hxx"
#include "ipp/ipp_exception.hxx"
#include <cstring>

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
	}

	void sixel_animation::initialize(int width, int height) {
		if (_encoder != nullptr) {
			return;
		}
		int result = sixel_allocator_new(&_allocator, &malloc, &calloc, &realloc, &free);
		if (SIXEL_FAILED(result)) {
			IPP_THROW_EXCEPTION(sixel_exception(sixel_helper_format_error(result)));
		}
		result = sixel_output_new(&_output, &sixel_write, stderr, _allocator);
		if (SIXEL_FAILED(result)) {
			IPP_THROW_EXCEPTION(sixel_exception(sixel_helper_format_error(result)));
		}
		result = sixel_dither_new(&_dither, 256, _allocator);
		if (SIXEL_FAILED(result)) {
			IPP_THROW_EXCEPTION(sixel_exception(sixel_helper_format_error(result)));
		}
		result = sixel_encoder_new(&_encoder, nullptr);
		if (SIXEL_FAILED(result)) {
			IPP_THROW_EXCEPTION(sixel_exception(sixel_helper_format_error(result)));
		}
		result = sixel_dither_initialize(_dither, _buf.get(), width, height,
		                                 SIXEL_PIXELFORMAT_G8, LARGE_AUTO, REP_AUTO, QUALITY_AUTO);
		if (SIXEL_FAILED(result)) {
			IPP_THROW_EXCEPTION(sixel_exception(sixel_helper_format_error(result)));
		}
	}

	void sixel_animation::data(uint8_t* mono_data, int line_height, int width, int height) {
		size_t expected_size = static_cast<std::size_t>(width * height);
		LOG(DEBUG) << expected_size;
		if (_buf_size < expected_size) {
			_buf_size = expected_size;
			_buf = std::make_unique<unsigned char[]>(expected_size);
		}
		unsigned char* ptr = _buf.get();
		for (int y = 0; y < height; ++y) {
			std::memcpy(ptr, mono_data + y * line_height, static_cast<std::size_t>(width));
			ptr += width;
		}
		initialize(width, height);
		int result = sixel_encode(_buf.get(), width, height, 8, _dither, _output);
		if (SIXEL_FAILED(result)) {
			IPP_THROW_EXCEPTION(sixel_exception(sixel_helper_format_error(result)));
		}
	}
}
