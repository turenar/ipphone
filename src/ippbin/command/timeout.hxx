#pragma once

#include "ippbin/terminal.hxx"

namespace ippbin {
	namespace command {
		void timeout_enable(terminal&, terminal::command_vector&&);
		void timeout_insert(terminal&, terminal::command_vector&&);
	}
}
