#pragma once

#include "ipp_exception.hxx"

namespace ipp {
	namespace protocol {
		struct protocol_exception : ipp_exception {
			protocol_exception(const std::string& s) : ipp_exception(s) {}
		};
	}
}
