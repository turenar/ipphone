#pragma once

#include <boost/exception/exception.hpp>
#include <system_error>

namespace ipp {
	namespace network {
		struct socket_exception : virtual std::system_error, virtual boost::exception {
			using std::system_error::system_error;
		};
	}
}
