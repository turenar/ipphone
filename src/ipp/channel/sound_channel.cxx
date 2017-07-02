#include "ipp/channel/sound_channel.hxx"
#include "ipp/ipphone.hxx"
#include "ipp/logger/logger.hxx"
#include "ipp/protocol/channel/sound_header.hxx"

namespace ipp {
	namespace channel {
		namespace {
			std::size_t c;
		}
		sound_channel::sound_channel(ipphone& ipp, uint32_t ch_id, channel_type type)
				: channel_wrapper(ch_id), _ipp(ipp), _ch_type(type) {
		}

		void sound_channel::receive(const std::uint8_t* buf, const std::uint16_t len) {
			using protocol::channel::sound_header;

			if (sizeof(sound_header) > len) {
				LOG(WARNING) << "corrupted channel packet";
				return;
			}
			const sound_header* header = reinterpret_cast<const sound_header*>(buf);
			const std::uint8_t* body = buf + sizeof(sound_header);
			LOG(DEBUG) << "receive samples: " << (c += header->samples);
			_ipp.writer_device()->buffer().write(reinterpret_cast<const std::uint16_t*>(body), header->samples << 1);
		}

		void sound_channel::flush_packets() {
			using protocol::channel::sound_header;
			std::uint8_t buf[sizeof(sound_header) + device::handler::default_buffered_samples * sizeof(std::uint16_t)];
			static_assert(sizeof(sound_header) == 4, "aho");
			buf[0] = static_cast<std::uint8_t>(protocol::channel::sound_encode_type::raw16);
			buf[1] = 0;

			const std::unique_ptr<device::handler>& dev = _ipp.reader_device();
			if (!dev) {
				return;
			}
			std::size_t len = dev->buffer().read(
					reinterpret_cast<std::uint16_t*>(buf + sizeof(sound_header)),
					device::handler::default_buffered_samples);
			if (len == 0) {
				return;
			}
			buf[2] = (len >> 1) & 0xff; // stereo..
			buf[3] = len >> 9;
			LOG(DEBUG) << "send samples: " << (c += len / 2);
			for (auto& pair : _ipp.connection_manager().get_connections()) {
				pair.second.protocol().channel_data(_channel_id, reinterpret_cast<std::uint8_t*>(buf),
				                                    sizeof(sound_header) + len * sizeof(std::uint16_t));
			}
		}

		void sound_channel::close() {
		}
	}
}
