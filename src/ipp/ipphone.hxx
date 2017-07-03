#pragma once

#include <memory>
#include <random>
#include "ipp/channel/channel_wrapper.hxx"
#include "ipp/device/handler.hxx"
#include "ipp/protocol/connection_manager.hxx"
#include "ipp/protocol/protocol_listener.hxx"
#include "ipp/protocol/message/message_channel.hxx"
#include "ipp/protocol/message/message_connection.hxx"
#include "sox/format.hxx"

namespace ipp {
	class ipphone : public protocol::protocol_listener {
	public:
		using channel_map_type = std::unordered_map<std::uint32_t, std::unique_ptr<channel::channel_wrapper>>;

		ipphone(bool reader_enabled = true);
		virtual ~ipphone();

		void bind(const std::string& ip, std::uint16_t port);
		void connect(const std::string& ip, std::uint16_t port);
		void update_frame();
		bool open_channel(protocol::channel::channel_type, protocol::channel::channel_flag);
		void open_channel(std::unique_ptr<channel::channel_wrapper>&&, protocol::channel::channel_flag fl);
		std::unique_ptr<channel::channel_wrapper> close_channel(const channel::channel_wrapper*);
		channel_map_type& channels();

		virtual void on_keep_alive(protocol::connection&, const protocol::message::keep_alive*, std::size_t) override;
		virtual void on_connect(protocol::connection&, const protocol::message::connect*, std::size_t) override;
		virtual void on_disconnect(protocol::connection&, const protocol::message::disconnect*, std::size_t) override;

		virtual void
		on_channel_open(protocol::connection&, const protocol::message::channel_open*, std::size_t) override;
		virtual void
		on_channel_data(protocol::connection&, const protocol::message::channel_data*, std::size_t) override;
		virtual void
		on_channel_close(protocol::connection&, const protocol::message::channel_close*, std::size_t) override;

		const std::unique_ptr<device::handler>& reader_device();
		const std::unique_ptr<device::handler>& writer_device();
		protocol::connection_manager& connection_manager();

	private:
		bool _reader_enabled;
		std::mt19937 _rnd;
		protocol::connection_manager _manager;
		channel_map_type _channels;
		std::unique_ptr<device::handler> _reader;
		std::unique_ptr<device::handler> _writer;
	};
}
