#include "ipp/ipphone.hxx"
#include <fstream>
#include "ipp/device/sox_read_handler.hxx"
#include "ipp/device/sox_write_handler.hxx"

namespace ipp {
	ipphone::ipphone(bool listen_mode) : _manager(network::socket(), *this) {
		_writer = std::make_unique<device::sox_write_handler>();
		if (listen_mode) {
			_reader = std::make_unique<device::sox_read_handler>();
		}
	}

	ipphone::~ipphone() {}

	void ipphone::bind(const std::string& ip, std::uint16_t port) {
		_manager.get_socket().bind(network::socket_address().set_address(ip.c_str()).set_port(port));
	}

	void ipphone::connect(const std::string& ip, std::uint16_t port) {
		network::socket_address addr;
		addr.set_address(ip.c_str()).set_port(port);
		_manager.connect(addr)
				.protocol().channel_open(1, protocol::message::channel_type::sound_stereo,
				                         protocol::message::channel_flag::receivable);
	}

	void ipphone::update_frame(bool listen) {
		_manager.consume_socket();
		std::uint16_t buf[48000 * 2 / 10];
		if (listen) {
			std::size_t len = _reader->buffer().read(buf, 4800 * 2);
			for (auto pair : _manager.get_connections()) {
				pair.second.protocol().channel_data(1, reinterpret_cast<std::uint8_t*>(buf),
				                                    static_cast<std::uint16_t>(len * sizeof(std::uint16_t)));
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	void ipphone::on_keep_alive(protocol::connection&, const protocol::message::keep_alive*, std::size_t) {}

	void ipphone::on_connect(protocol::connection&, const protocol::message::connect*, std::size_t) {}

	void ipphone::on_disconnect(protocol::connection&, const protocol::message::disconnect*, std::size_t) {}

	void ipphone::on_channel_open(protocol::connection&, const protocol::message::channel_open*, std::size_t) {}

	void ipphone::on_channel_data(protocol::connection&, const protocol::message::channel_data* dat, std::size_t len) {
		const std::uint8_t* pointer = reinterpret_cast<const std::uint8_t*>(dat) + sizeof(*dat);
		_writer->buffer().write(reinterpret_cast<const std::uint16_t*>(pointer),
		                        (len - sizeof(*dat)) / sizeof(std::uint16_t));
	}

	void ipphone::on_channel_close(protocol::connection&, const protocol::message::channel_close*, std::size_t) {}
}
