#pragma once

#include <boost/exception/detail/error_info_impl.hpp>

namespace ipp {
	namespace internal {
		struct backtrace_tag {
		};

		std::ostream& backtrace_info(std::ostream& os);
	}
}
namespace boost {
	template <>
	class error_info<::ipp::internal::backtrace_tag, ::ipp::internal::backtrace_tag>
			: public boost::exception_detail::error_info_base {
	public:
		error_info();

		virtual std::string name_value_string() const override { return _s; }

	private:
		std::string _s;
	};
}
namespace ipp {
	using errinfo_ipp_backtrace = ::boost::error_info<::ipp::internal::backtrace_tag, ::ipp::internal::backtrace_tag>;
}
