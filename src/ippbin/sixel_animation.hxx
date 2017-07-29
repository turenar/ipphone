#pragma once

#include <sixel.h>
#include <memory>

namespace ippbin {
	class sixel_animation {
	public:
		sixel_animation() = default;
		sixel_animation(const sixel_animation&) = delete;
		sixel_animation(sixel_animation&&) = delete;
		sixel_animation& operator=(const sixel_animation&) = delete;
		sixel_animation& operator=(sixel_animation&&) = delete;
		~sixel_animation();

		void data(uint8_t* mono_data, int line_height, int width, int height);

	private:
		sixel_allocator_t* _allocator = nullptr;
		sixel_encoder_t* _encoder = nullptr;
		sixel_dither_t* _dither = nullptr;
		sixel_output_t* _output = nullptr;
		std::unique_ptr<unsigned char[]> _buf;
		std::size_t _buf_size = 0;

		void initialize(int width, int height);
	};
}
