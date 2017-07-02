#include <array>
#include "sox/format.hxx"
#include "ipp/device/sox_read_handler.hxx"
#include "ipp/logger/logger.hxx"

namespace ipp {
	namespace device {
		namespace {
			struct read_worker {
				read_worker(sox_read_handler& handler, sox::format&& format)
						: _handler(handler), _format(std::move(format)) {
				}

				void operator()() {
					handler::buffer_t& buffer = _handler.buffer();
					std::array<sox::sample_t, handler::default_buffered_samples> buf;
					std::array<std::uint16_t, handler::default_buffered_samples> encoded;
					std::size_t count = 0;

					while (!_handler.shutting_down()) {
						std::size_t read_len = _format.read(buf.begin(), buf.size());
						for (std::size_t i = 0; i < read_len; i++) {
							encoded[i] = static_cast<std::uint16_t>(buf[i] >> 16);
						}
						buffer.write(encoded.begin(), read_len);
						LOG(DEBUG) << '[' << (count += read_len) << "] read_len " << read_len;
					}
				}

				sox_read_handler& _handler;
				sox::format _format;
			};
		}

		sox_read_handler::sox_read_handler() {
			_worker = std::thread(
					read_worker(*this, sox::format::open_device_for_read("default", nullptr, nullptr, "pulseaudio")));
		}

		bool sox_read_handler::shutting_down() const {
			return false; // FIXME
		}
	}
}