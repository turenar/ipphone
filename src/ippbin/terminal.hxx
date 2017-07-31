#pragma once

#include <chrono>
#include <functional>
#include <ncurses.h>
#include <string>
#include <unordered_map>
#include <vector>
#include "ipp/ipphone.hxx"
#include "ippbin/sixel_animation.hxx"

namespace ippbin {
	class terminal {
	public:
		using command_vector = std::vector<std::string>;
		using command_callback_type = std::function<void(terminal&, command_vector&&)>;

		static constexpr int color_pair_default = 0;
		static constexpr int color_pair_output = 1;
		static constexpr int color_pair_error = 2;
		static constexpr int color_pair_warning = 2;
		static constexpr int timeout_warn_sec = 5;

		terminal();
		terminal(const terminal&) = delete;
		terminal(terminal&&) = delete;
		terminal& operator=(const terminal&)= delete;
		terminal& operator=(terminal&&) = delete;
		~terminal();

		void run();
		bool loop();

		std::string readline();

		void register_command(std::string name, command_callback_type callback);
		bool parse_command(const std::string&);

		void enable_timeout();
		bool check_timeout();
		void add_timeout(std::size_t sec);

		void println(const std::string&, int color_pair = color_pair_output);

		ipp::ipphone& ipp();

	private:
		using time_point_type = std::chrono::time_point<
				std::chrono::steady_clock,
				std::chrono::duration<long, std::chrono::seconds::period>>;

		WINDOW* _win;
		bool _console_enabled = true;
		bool _timeout_enabled = false;
		bool _timeout_warned = false;
		std::chrono::steady_clock::time_point _start;
		std::size_t _timeout_sec;
		std::unordered_map<std::string, command_callback_type> _commands;
		std::string _buf;

		ipp::ipphone _ipp;
		sixel_animation _sixel;

		void channel_open_hook(std::unique_ptr<ipp::channel::channel_wrapper>&, ipp::protocol::channel::channel_flag);
		void video_frame_hook(const AVFrame* fr, int width, int height);
	};
}
