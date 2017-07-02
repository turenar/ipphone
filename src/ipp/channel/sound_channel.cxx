#include "ipp/channel/sound_channel.hxx"
#include "ipp/ipphone.hxx"
#include "ipp/logger/logger.hxx"
#include "ipp/protocol/channel/sound_header.hxx"

namespace ipp {
	namespace channel {
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
			_ipp.writer_device()->buffer().write(reinterpret_cast<const std::uint16_t*>(body), header->samples);
		}

		void sound_channel::flush_packets() {
			const std::unique_ptr<device::handler>& dev = _ipp.reader_device();
			if (!dev) {
				return;
			}
			std::uint32_t ch_id = 0;
			for (auto& channels : _ipp.channels()) {
				if (channels.second->type() == protocol::channel::channel_type::sound) {
					ch_id = channels.second->ch_id();
				}
			}
			while (true) {
				std::uint16_t buf[48000 * 2 / 10];
				std::size_t len = dev->buffer().read(buf, 4800 * 2);
				if (len == 0) {
					break;
				}

				for (auto& pair : _ipp.connection_manager().get_connections()) {
					pair.second.protocol().channel_data(ch_id, reinterpret_cast<std::uint8_t*>(buf),
					                                    static_cast<std::uint16_t>(len * sizeof(std::uint16_t)));
				}
			}
		}

		void sound_channel::close() {
		}
	}
}
