#pragma once

#include "ipp/ipp_exception.hxx"

namespace ippbin {
	namespace command {
		struct command_exception : ipp::ipp_exception {
			command_exception(const std::string& s) : ipp_exception(s) {}
		};
	}
}
