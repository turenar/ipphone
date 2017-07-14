#pragma once

#include <array>
#include <functional>
#include <fstream>
#include "ipp/channel/channel_wrapper.hxx"

namespace ipp {
	class ipphone;
	namespace channel {
		class file_channel final : public channel_wrapper {
		public:
			using channel_type = protocol::channel::channel_type;
			using callback_type = std::function<void(const std::string&, bool)>;

			file_channel(ipphone&, uint32_t ch_id, channel_type ch_type, std::string file_name = "");

			virtual void receive(const std::uint8_t* buf, const std::uint16_t len) override;
			virtual void flush_packets() override;
			virtual void close() override;

			void set_callback(callback_type);

		protected:
			ipphone& _ipp;
			std::fstream _fs;
			std::string _file_name;
			callback_type _callback;
			bool _send_mode;
			bool _file_name_notified = false;
		};
	}
}
