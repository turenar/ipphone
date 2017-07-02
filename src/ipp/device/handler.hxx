#pragma once

#include "ipp/util/buffer.hxx"

namespace ipp {
	namespace device {
		class handler {
		public:
			using buffer_t = util::buffer<std::uint16_t>;
			static constexpr std::size_t samples_per_second = 48000;
			static constexpr std::size_t channels = 2;
			static constexpr std::size_t default_buffered_samples = 4096 * channels;
			static constexpr std::size_t default_buffer_size = default_buffered_samples * 64;

			handler(std::size_t buffer_size = default_buffer_size);

			virtual ~handler() {}

			buffer_t& buffer();

		protected:
			util::buffer<std::uint16_t> _buf;
		};
	}
}
