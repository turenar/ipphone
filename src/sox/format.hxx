#pragma once

#include <string>
extern "C" {
#include "sox.h"
}

namespace sox {
	using sample_t = ::sox_sample_t;
	using signalinfo = ::sox_signalinfo_t;
	using encodinginfo = ::sox_encodinginfo_t;
	using oob = ::sox_oob_t;

	class format {
	public:
		static format open_device_for_read(const std::string& path, const signalinfo* signal, const encodinginfo* encoding,
		                                   const std::string& filetype);
		static format open_device_for_write(const std::string& path, const signalinfo* signal, const encodinginfo* encoding,
		                                    const std::string& filetype, const oob* oob);
		format();
		format(::sox_format_t*);
		format(const format&) = delete;
		format(format&&) noexcept;
		format& operator=(const format&) = delete;
		format& operator=(format&&) noexcept;
		~format();

		std::size_t read(sample_t* buf, std::size_t len);
		std::size_t write(const sample_t* buf, std::size_t len);

	private:
		::sox_format_t* _format;
	};
}
