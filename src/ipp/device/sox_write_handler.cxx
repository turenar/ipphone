#include "ipp/device/sox_write_handler.hxx"
#include <array>
#include <fstream>
#include "ipp/logger/logger.hxx"
#include "sox/format.hxx"

namespace ipp {
	namespace device {
		namespace {
			struct write_worker {
				write_worker(sox_write_handler& handler, sox::format&& format)
						: _handler(handler), _format(std::move(format)) {
				}

				void operator()() {
					handler::buffer_t& buffer = _handler.buffer();
					std::array<std::uint16_t, handler::default_buffered_samples> encoded;
					std::array<sox::sample_t, handler::default_buffered_samples> buf;
					std::size_t count = 0;
					while (!_handler.shutting_down()) {
						std::size_t read_len = buffer.read(encoded.begin(), encoded.size(),
						                                   std::chrono::milliseconds(20));
						if (read_len > 0) {
							for (std::size_t i = 0; i < read_len; i++) {
								buf[i] = encoded[i] << 16;
							}
							_format.write(buf.begin(), read_len);
							LOG(DEBUG) << '[' << (count += read_len) << "] written len " << read_len;
						}
					}
				}

				sox_write_handler& _handler;
				sox::format _format;
			};
		}

		sox_write_handler::sox_write_handler() {
			sox::signalinfo sig;
			sig.channels = channels;
			sig.length = 0;
			sig.precision = 0;
			sig.rate = samples_per_second;
			_worker = std::thread(
					write_worker(*this,
					             sox::format::open_device_for_write("default", &sig, nullptr, "pulseaudio", nullptr)));
		}

		sox_write_handler::~sox_write_handler() {
			shutdown();
			_worker.join();
		}

		void sox_write_handler::shutdown() {
			_shutting_down = true;
		}

		bool sox_write_handler::shutting_down() const {
			return _shutting_down; // FIXME
		}
	}
}
