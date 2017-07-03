#include "ippbin/terminal.hxx"
#include <iostream>
#include "ipp/logger/logger.hxx"

namespace ippbin {
	constexpr int terminal::timeout_warn;

	terminal::terminal() {
	}

	void terminal::run() {
		_win = initscr();
		start_color();
		cbreak();
		noecho();
		init_pair(color_pair_output, 6, 0);
		init_pair(color_pair_error, 1, 0);
		init_pair(color_pair_warning, 3, 0);
		nodelay(_win, true);
		keypad(_win, true);
		while (loop()) {
			// blur blur..
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
		endwin();
		std::cout << "Bye." << std::endl;
	}

	bool terminal::loop() {
		std::string line = readline();
		if (!line.empty()) {
			parse_command(line);
		}

		return !check_timeout();
	}

	void terminal::parse_command(const std::string& cmd) {
		println(cmd, color_pair_default);
		if (cmd == "timeout") {
			_timeout_enabled = true;
			_timeout_warned = false;
			_timeout = std::chrono::seconds(10);
			_start = std::chrono::steady_clock::now();
			println("timeout enabled");
		} else {
			println(std::string("unknown command: ") + cmd, color_pair_error);
		}
	}

	bool terminal::check_timeout() {
		auto now = std::chrono::steady_clock::now();
		if (_timeout_enabled && !_timeout_warned && (now - _start) >= std::chrono::seconds(timeout_warn)) {
			println("まもなく切断されます。お金を入れてください。", color_pair_warning);
			_timeout_warned = true;
		}
		return _timeout_enabled && (now - _start) > _timeout;
	}

	std::string terminal::readline() {
		int c;
		int h = getmaxy(_win);
		while (ERR != (c = wgetch(_win))) {
			switch (c) {
				case KEY_BACKSPACE:
				case 127: // delete
					if (!_buf.empty()) {
						_buf.erase(_buf.length() - 1);
					}
					break;
				case KEY_ENTER:
				case '\n': {
					std::string d = std::move(_buf);
					return d;
				}
				default:
					if (c < KEY_MIN) {
						_buf += c;
					}
			}
		}
		// restore buffer
		wmove(_win, h - 1, 0);
		clrtoeol();
		wprintw(_win, "%s", _buf.c_str());
		wmove(_win, h - 1, static_cast<int>(_buf.length()));
		wrefresh(_win);
		return "";
	}

	void terminal::println(const std::string& line, int color_pair) {
		int h = getmaxy(_win);
		wmove(_win, 0, 0);
		deleteln();
//		mvwprintw(_output, h, 0, "%s", line.c_str());
		attron(COLOR_PAIR(color_pair));
		wmove(_win, h - 2, 0);
		clrtoeol();
		wprintw(_win, "%s", line.c_str());
		attroff(COLOR_PAIR(color_pair));

		wrefresh(_win);
	}
}



