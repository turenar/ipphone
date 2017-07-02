#include "ipp/ipphone.hxx"
#include <chrono>
#include <fstream>
#include "ipp/channel/sound_channel.hxx"
#include "ipp/device/sox_read_handler.hxx"
#include "ipp/device/sox_write_handler.hxx"

namespace ipp {
	namespace {
		std::unique_ptr<channel::channel_wrapper>
		create_channel(ipphone& ipp, std::uint32_t ch_id, protocol::channel::channel_type ty) {
			switch (ty) {
				case protocol::channel::channel_type::sound:
					return std::make_unique<channel::sound_channel>(ipp, ch_id, ty);
				default:
					return nullptr;
			}
		}
	}

	ipphone::ipphone(bool listen_mode) : _manager(network::socket(), *this) {
		_rnd.seed(static_cast<unsigned long>(std::chrono::system_clock::now().time_since_epoch().count()));
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
				.protocol().channel_open(1, protocol::channel::channel_type::sound,
				                         protocol::channel::channel_flag::none);
	}

	void ipphone::update_frame(bool listen) {
		_manager.consume_socket();
		for (auto& channel : _channels) {
			channel.second->flush_packets();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}

	bool ipphone::open_channel(protocol::channel::channel_type ty, protocol::channel::channel_flag fl) {
		uint32_t ch_id = static_cast<std::uint32_t>(_rnd());
		auto ch = create_channel(*this, ch_id, ty);
		if (ch) {
			_channels.emplace(ch_id, std::move(ch));
			for (auto& connection : _manager.get_connections()) {
				connection.second.protocol().channel_open(ch_id, ty, fl);
			}
			return true;
		}
		return false;
	}

	ipphone::channel_map_type& ipphone::channels() {
		return _channels;
	}

	void ipphone::on_keep_alive(protocol::connection&, const protocol::message::keep_alive*, std::size_t) {}

	void ipphone::on_connect(protocol::connection&, const protocol::message::connect*, std::size_t) {}

	void ipphone::on_disconnect(protocol::connection&, const protocol::message::disconnect*, std::size_t) {}

	void ipphone::on_channel_open(protocol::connection&, const protocol::message::channel_open* dat, std::size_t) {
		auto ch = create_channel(*this, dat->ch_id, dat->ch_type);
		if (ch) {
			uint32_t ch_id = ch->ch_id();
			_channels.emplace(ch_id, std::move(ch));
		}
	}

	void ipphone::on_channel_data(protocol::connection&, const protocol::message::channel_data* dat, std::size_t len) {
		_channels.find(dat->ch_id);
		const std::uint8_t* pointer = reinterpret_cast<const std::uint8_t*>(dat) + sizeof(*dat);
		_writer->buffer().write(reinterpret_cast<const std::uint16_t*>(pointer),
		                        (len - sizeof(*dat)) / sizeof(std::uint16_t));
	}

	void ipphone::on_channel_close(protocol::connection&, const protocol::message::channel_close*, std::size_t) {}

	const std::unique_ptr<device::handler>& ipphone::reader_device() {
		return _reader;
	}

	const std::unique_ptr<device::handler>& ipphone::writer_device() {
		return _writer;
	}

	protocol::connection_manager& ipphone::connection_manager() {
		return _manager;
	}
}
