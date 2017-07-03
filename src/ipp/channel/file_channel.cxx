#include "ipp/channel/file_channel.hxx"
#include "ipp/ipphone.hxx"
#include "ipp/logger/logger.hxx"
#include "ipp/network/socket_exception.hxx"
#include "ipp/protocol/channel/sound_header.hxx"

namespace ipp {
	namespace channel {
		file_channel::file_channel(ipphone& ipp, uint32_t ch_id, channel_type type, std::string file_name)
				: channel_wrapper(ch_id), _ipp(ipp), _ch_type(type) {
			// prepare device
			ipp.reader_device();
			ipp.writer_device();
			_send_mode = !file_name.empty();
			if (_send_mode) {
				_fs.open(file_name, std::ios::in | std::ios::binary);
				if (!_fs) {
					IPP_THROW_EXCEPTION(network::socket_exception(errno, std::system_category()));
				}
			}
			_file_name = basename(file_name.c_str());
		}

		void file_channel::receive(const std::uint8_t* buf, const std::uint16_t len) {
			if (_send_mode) {
				LOG(ERROR) << "send mode but data received";
				return;
			}

			const std::uint8_t flag = *buf++;
			if (flag == 0) {
				const std::size_t name_len = *buf++;
				std::string file_name{reinterpret_cast<const char*>(buf), name_len};
				file_name += '.';
				file_name += std::to_string(getpid());
				LOG(INFO) << "opened " << file_name;
				_fs.open(file_name, std::ios::out | std::ios::binary | std::ios::trunc);
			} else {
				const std::uint8_t* body = buf;
				std::size_t body_len = len - 1;
				_fs.write(reinterpret_cast<const char*>(body), body_len);
			}
		}

		void file_channel::flush_packets() {
			using protocol::channel::sound_header;
			if (!_send_mode || !_fs) {
				return;
			}

			std::uint8_t buf[1024];
			if (!_file_name_notified) {
				buf[0] = 0;
				buf[1] = static_cast<std::uint8_t>(_file_name.length());
				std::memcpy(buf + 2, _file_name.c_str(), _file_name.length() + 1);
				for (auto& pair : _ipp.connection_manager().get_connections()) {
					pair.second->protocol().channel_data(_channel_id, buf,
					                                     static_cast<std::uint16_t>( _file_name.length() + 3));
				}
				_file_name_notified = true;
			}

			buf[0] = 1;
			std::streamsize read_len = _fs.readsome(reinterpret_cast<char*>(buf + 1), sizeof(buf) - 1);
			if (read_len == 0) {
				_fs.read(reinterpret_cast<char*>(buf + 1), 1);
				read_len = _fs.gcount();
			}
			if (read_len > 0) {
				for (auto& pair : _ipp.connection_manager().get_connections()) {
					pair.second->protocol().channel_data(_channel_id, reinterpret_cast<std::uint8_t*>(buf),
					                                     static_cast<std::uint16_t>(read_len + 1));
				}
			}
			if (_fs.eof()) {
				for (auto& pair : _ipp.connection_manager().get_connections()) {
					pair.second->protocol().channel_close(_channel_id);
				}
				LOG(INFO) << "file transfer completed";
			}
		}

		void file_channel::close() {
			_fs.close();
		}
	}
}
