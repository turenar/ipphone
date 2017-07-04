#pragma once

#include "ipp/ipp_exception.hxx"

namespace ipp {
	struct ipp_system_exception : ipp_exception {
		ipp_system_exception(int ec, const std::error_category& cat)
				: ipp_exception(std::error_code(ec, cat).message()) {
		}

		ipp_system_exception(const std::string& prefix, int ec, const std::error_category& cat)
				: ipp_exception(prefix + ": " + std::error_code(ec, cat).message()) {
		}
	};
}
