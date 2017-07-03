#include "ippbin/command/command.hxx"
#include "ippbin/command/command_exception.hxx"

namespace ippbin {
	namespace command {
		namespace {
			std::uint16_t to_port(const std::string& str) {
				char* idx;
				long p = std::strtol(str.c_str(), &idx, 10);
				if (*idx != '\0' || p < 1024 || p > std::numeric_limits<std::uint16_t>::max()) {
					throw command_exception(std::string("wrong port: ") + str);
				}
				return static_cast<std::uint16_t>( p);
			}
		}

		void phone_listen(terminal& t, terminal::command_vector&& args) {
			std::uint16_t port = 12345;
			if (args.size() >= 2) {
				port = to_port(args[1]);
			}
			t.ipp().bind("0.0.0.0", port);
			t.println("listening at port " + std::to_string(port));
		}

		void phone_connect(terminal& t, terminal::command_vector&& args) {
			if (args.size() <= 1) {
				throw command_exception("usage: connect <ipaddr> [<port>]");
			}
			std::uint16_t port = 12345;
			if (args.size() >= 3) {
				port = to_port(args[2]);
			}
			t.ipp().connect(args[1], port);
			t.ipp().open_channel(ipp::protocol::channel::channel_type::sound,
			                     ipp::protocol::channel::channel_flag::none);;
			t.println(std::string("connected ") + args[1] + ':' + std::to_string(port));
		}

		void phone_debug(terminal& t, terminal::command_vector&&) {
			t.ipp().enable_reader(false);
			t.println("enabled debug feature");
		}
	}
}
