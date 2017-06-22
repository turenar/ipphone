#pragma once

#include "ipp_exception.hxx"

namespace ipp {
	namespace network {
		struct socket_exception : ipp_exception {
			socket_exception(int ec, const std::error_category& cat)
					: ipp_exception(std::error_code(ec, cat).message()) {
			}
		};
	}
}
