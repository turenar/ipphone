#pragma once

#include <chrono>
#include <ncurses.h>
#include <string>
#include <unordered_map>

namespace ippbin {
	class terminal {
	public:
		static constexpr int color_pair_default = 0;
		static constexpr int color_pair_output = 1;
		static constexpr int color_pair_error = 2;
		static constexpr int color_pair_warning = 2;
		static constexpr int timeout_warn = 5;

		terminal();

		void run();
		bool loop();

		std::string readline();
		void parse_command(const std::string&);
		bool check_timeout();

		void println(const std::string&, int color_pair = color_pair_output);

	private:
		WINDOW* _win;
		bool _timeout_enabled = false;
		bool _timeout_warned = false;
		std::chrono::seconds _timeout;
		std::chrono::steady_clock::time_point _start;
		std::string _buf;
	};
}
