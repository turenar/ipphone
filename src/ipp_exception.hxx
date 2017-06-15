#pragma once

#include <sstream>
#include <exception>
#include <boost/exception/exception.hpp>
#include <boost/exception/info.hpp>
#include <boost/exception/detail/error_info_impl.hpp>
#include "error_info.hxx"

namespace ipp {
	class ipp_exception : public ::std::exception, public ::boost::exception {
	public:
		explicit ipp_exception(std::string s) : _what(std::move(s)) {}

		virtual const char* what() const noexcept override { return _what.c_str(); }

		virtual ~ipp_exception() {}

	private:
		std::string _what;
	};
}

#ifdef NDEBUG
#define IPP_THROW_EXCEPTION(ex) BOOST_THROW_EXCEPTION((ex))
#else
#define IPP_THROW_EXCEPTION(ex) BOOST_THROW_EXCEPTION((ex) << ::ipp::errinfo_ipp_backtrace())
#endif
