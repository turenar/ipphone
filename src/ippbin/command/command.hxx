#pragma once

#include "ippbin/terminal.hxx"

namespace ippbin {
	namespace command {
		void timeout_enable(terminal&, terminal::command_vector&&);
		void timeout_insert(terminal&, terminal::command_vector&&);

		void phone_listen(terminal&, terminal::command_vector&&);
		void phone_connect(terminal&, terminal::command_vector&&);
		void phone_debug(terminal&, terminal::command_vector&&);
		void phone_sendfile(terminal&, terminal::command_vector&&);
		void phone_video(terminal&, terminal::command_vector&&);
	}
}
