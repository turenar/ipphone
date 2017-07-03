#include "ipp/ipphone.hxx"
#include <chrono>
#include <fstream>
#include "ipp/channel/file_channel.hxx"
#include "ipp/channel/sound_channel.hxx"
#include "ipp/device/sox_read_handler.hxx"
#include "ipp/device/sox_write_handler.hxx"

namespace ipp {
	namespace {
		std::unique_ptr<channel::channel_wrapper>
		create_channel(ipphone& ipp, std::uint32_t ch_id, protocol::channel::channel_type ty,
		               protocol::channel::channel_flag = protocol::channel::channel_flag::none) {
			LOG(WARNING) << "creating channel" << ch_id;
			switch (ty) {
				case protocol::channel::channel_type::sound:
					return std::make_unique<channel::sound_channel>(ipp, ch_id, ty);
				case protocol::channel::channel_type::file:
					return std::make_unique<channel::file_channel>(ipp, ch_id, ty);
				default:
					return nullptr;
			}
		}
	}

	ipphone::ipphone(bool reader_enabled) : _manager(network::socket(), *this) {
		_rnd.seed(static_cast<unsigned long>(std::chrono::system_clock::now().time_since_epoch().count()));
		_writer = std::make_unique<device::sox_write_handler>();
		_reader_enabled = reader_enabled;
	}

	ipphone::~ipphone() {}

	void ipphone::bind(const std::string& ip, std::uint16_t port) {
		_manager.get_socket().bind(network::socket_address().set_address(ip.c_str()).set_port(port));
	}

	void ipphone::connect(const std::string& ip, std::uint16_t port) {
		network::socket_address addr;
		addr.set_address(ip.c_str()).set_port(port);
		_manager.connect(addr);
	}

	void ipphone::update_frame() {
		_manager.consume_socket();
		for (auto& channel : _channels) {
			channel.second->flush_packets();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	bool ipphone::open_channel(protocol::channel::channel_type ty, protocol::channel::channel_flag fl) {
		uint32_t ch_id = static_cast<std::uint32_t>(_rnd());
		auto ch = create_channel(*this, ch_id, ty, fl);
		if (ch) {
			_channels.emplace(ch_id, std::move(ch));
			for (auto& connection : _manager.get_connections()) {
				connection.second->protocol().channel_open(ch_id, ty, fl);
			}
			return true;
		} else {
			LOG(ERROR) << "channel_type is not found" << static_cast<int>(ty);
			return false;
		}
	}

	void ipphone::open_channel(std::unique_ptr<channel::channel_wrapper>&& ptr, protocol::channel::channel_flag fl) {
		uint32_t ch_id = static_cast<std::uint32_t>(_rnd());
		ptr->ch_id(ch_id);
		channel::channel_wrapper::channel_type type = ptr->type();
		_channels.emplace(ch_id, std::move(ptr));
		for (auto& connection : _manager.get_connections()) {
			connection.second->protocol().channel_open(ch_id, type, fl);
		}
	}


	ipphone::channel_map_type& ipphone::channels() {
		return _channels;
	}

	void ipphone::on_keep_alive(protocol::connection&, const protocol::message::keep_alive*, std::size_t) {}

	void ipphone::on_connect(protocol::connection&, const protocol::message::connect*, std::size_t) {}

	void ipphone::on_disconnect(protocol::connection&, const protocol::message::disconnect*, std::size_t) {}

	void ipphone::on_channel_open(protocol::connection&, const protocol::message::channel_open* dat, std::size_t) {
		auto it = _channels.find(dat->ch_id);
		if (it != _channels.end()) {
			return;
		}
		auto ch = create_channel(*this, dat->ch_id, dat->ch_type);
		if (ch) {
			uint32_t ch_id = ch->ch_id();
			_channels.emplace(ch_id, std::move(ch));
			for (auto& connection : _manager.get_connections()) {
				connection.second->protocol().channel_open(ch_id, dat->ch_type, dat->ch_flag);
			}
		}
	}

	void ipphone::on_channel_data(protocol::connection&, const protocol::message::channel_data* dat, std::size_t len) {
		auto it = _channels.find(dat->ch_id);
		if (it != _channels.end()) {
			it->second->receive(reinterpret_cast<const std::uint8_t*>(dat) + sizeof(*dat),
			                    static_cast<std::uint16_t>(len - sizeof(*dat)));
		} else {
			LOG(WARNING) << "channel id?" << dat->ch_id;
		}
	}

	void ipphone::on_channel_close(protocol::connection&, const protocol::message::channel_close* dat, std::size_t) {
		auto it = _channels.find(dat->ch_id);
		if (it == _channels.end()) {
			return;
		}
		LOG(INFO) << "close channel" << dat->ch_id;
		it->second->close();
		_channels.erase(it);
		for (auto& connection : _manager.get_connections()) {
			connection.second->protocol().channel_close(dat->ch_id);
		}
	}

	const std::unique_ptr<device::handler>& ipphone::reader_device() {
		if (!_reader && _reader_enabled) {
			_reader = std::make_unique<device::sox_read_handler>();
		}
		return _reader;
	}

	const std::unique_ptr<device::handler>& ipphone::writer_device() {
		return _writer;
	}

	protocol::connection_manager& ipphone::connection_manager() {
		return _manager;
	}
}
