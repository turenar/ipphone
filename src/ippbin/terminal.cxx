#include "ippbin/terminal.hxx"
#include <iostream>
#include "ipp/logger/logger.hxx"
#include "ippbin/command/command.hxx"
#include "ippbin/command/command_exception.hxx"
#include "ipp/channel/file_channel.hxx"
#include "ipp/channel/video_decoder_channel.hxx"

namespace ippbin {
	namespace {
		struct exit_exception : command::command_exception {
			exit_exception() : command_exception("") {}
		};

		void exit_terminal(terminal&, terminal::command_vector&&) {
			throw exit_exception();
		}

		void register_commands(terminal& t) {
			t.register_command("enable_timeout", command::timeout_enable);
			t.register_command("10yen", command::timeout_insert);
			t.register_command("100yen", command::timeout_insert);
			t.register_command("5000000000000000yen", command::timeout_insert);
			t.register_command("listen", command::phone_listen);
			t.register_command("connect", command::phone_connect);
			t.register_command("debug", command::phone_debug);
			t.register_command("sendfile", command::phone_sendfile);
			t.register_command("video", command::phone_video);
			t.register_command("quit", &exit_terminal);
			t.register_command("exit", &exit_terminal);
			t.register_command("q", &exit_terminal);
		}
	}

	constexpr int terminal::timeout_warn_sec;

	terminal::terminal() {
		using namespace std::placeholders;
		register_commands(*this);
		_ipp.open_channel_callback(
				ipp::ipphone::channel_open_callback(std::bind(&terminal::channel_open_hook, this, _1, _2)));
	}

	terminal::~terminal() {
		if (!isendwin()) {
			endwin();
		}
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
			_ipp.update_frame();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		if (!isendwin()) {
			endwin();
		}
		std::cout << "Bye." << std::endl;
	}

	bool terminal::loop() {
		if (!_console_enabled) {
			return true;
		}
		std::string line = readline();
		if (!line.empty()) {
			if (!parse_command(line)) {
				return false;
			}
		}

		return !check_timeout();
	}

	bool terminal::parse_command(const std::string& cmd) {
		println(cmd, color_pair_default);

		std::size_t argc = static_cast<std::size_t>( std::count(cmd.cbegin(), cmd.cend(), ' '));
		std::vector<std::string> args;
		args.reserve(argc);

		std::size_t arg_first_char = 0;
		while (true) {
			std::size_t space_index = cmd.find(' ', arg_first_char);
			if (space_index == std::string::npos) {
				args.emplace_back(cmd, arg_first_char);
				break;
			} else if (space_index == arg_first_char + 1) {
				// space sequenced
				continue;
			} else {
				args.emplace_back(cmd, arg_first_char, space_index - arg_first_char);
				arg_first_char = space_index + 1; // space
			}
		}
		if (args.empty()) {
			// no command
			return true;
		}
		auto it = _commands.find(args[0]);
		if (it != _commands.end()) {
			try {
				it->second(*this, std::move(args));
			} catch (exit_exception&) {
				return false;
			} catch (command::command_exception& ex) {
				println(ex.what(), color_pair_error);
			}
		} else {
			println(std::string("unknown command: ") + cmd, color_pair_error);
		}
		return true;
	}

	void terminal::enable_timeout() {
		_timeout_enabled = true;
		_timeout_warned = false;
		_start = std::chrono::steady_clock::now();
		_timeout_sec = 10;
		println("timeout enabled");
	}

	bool terminal::check_timeout() {
		auto now = std::chrono::steady_clock::now();
		auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now - _start);
		if (_timeout_enabled && !_timeout_warned && seconds >= std::chrono::seconds(_timeout_sec - timeout_warn_sec)) {
			println("まもなく切断されます。お金を入れてください。", color_pair_warning);
			_timeout_warned = true;
		}
		return _timeout_enabled && seconds > std::chrono::seconds(_timeout_sec);
	}

	void terminal::add_timeout(std::size_t sec) {
		if (!_timeout_enabled) {
			println("type 'enable_timeout' first");
			return;
		}
		_timeout_warned = false;
		_timeout_sec += sec;

		auto now = std::chrono::steady_clock::now();
		auto remain_sec = _timeout_sec - std::chrono::duration_cast<std::chrono::seconds>(now - _start).count();
		println(std::string("残り通話可能時間は ") + std::to_string(remain_sec) + " 秒です");
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

	void terminal::register_command(std::string name, command_callback_type callback) {
		_commands.emplace(std::move(name), std::move(callback));
	}

	ipp::ipphone& terminal::ipp() { return _ipp; }

	void
	terminal::channel_open_hook(std::unique_ptr<ipp::channel::channel_wrapper>& ptr,
	                            ipp::protocol::channel::channel_flag) {
		ipp::channel::channel_wrapper* p = ptr.get();
		auto fc = dynamic_cast<ipp::channel::file_channel*>(p);
		if (fc) {
			println("incoming file channel");
			fc->set_callback(
					[this](const std::string& filename, bool) { this->println("transfer done: " + filename); });
		}
		auto vdc = dynamic_cast<ipp::channel::video_decoder_channel*>(p);
		if (vdc) {
			using namespace std::placeholders;
			println("incoming video channel");
			vdc->callback(std::bind(&terminal::video_frame_hook, this, _1, _2, _3));
			_console_enabled = false;
			endwin();
		}
	}

	void terminal::video_frame_hook(const AVFrame* fr, int width, int height) {
		_sixel.data(fr, width, height);
	}
}
