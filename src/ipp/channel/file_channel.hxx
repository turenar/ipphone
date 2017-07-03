#pragma once

#include <array>
#include <fstream>
#include "ipp/channel/channel_wrapper.hxx"

namespace ipp {
	class ipphone;
	namespace channel {
		class file_channel : public channel_wrapper {
			static constexpr std::size_t buffer_packet = 16;
		public:
			using channel_type = protocol::channel::channel_type;

			file_channel(ipphone&, uint32_t ch_id, channel_type ch_type, std::string file_name = "");

			virtual channel_type type() const override {
				return _ch_type;
			}

			virtual void receive(const std::uint8_t* buf, const std::uint16_t len) override;
			virtual void flush_packets() override;
			virtual void close() override;

		protected:
			ipphone& _ipp;
			channel_type _ch_type;
			std::fstream _fs;
			std::string _file_name;
			bool _send_mode;
			bool _file_name_notified = false;
		};
	}
}
