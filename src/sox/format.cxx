#include "sox/format.hxx"

namespace sox {
	format format::open_device_for_read(const std::string& path, const signal* signal, const encodinginfo* encoding,
	                                    const std::string& filetype) {
		return format(::sox_open_read(path.c_str(), signal, encoding, filetype.c_str()));
	}

	format::format() {
		_format = nullptr;
	}


	format::format(::sox_format_t* f) {
		_format = f;
	}

	format::format(format&& other) noexcept {
		*this = std::move(other);
	}

	format& format::operator=(format&& other) noexcept {
		_format = other._format;
		other._format = nullptr;
		return *this;
	}

	format::~format() {
		if (_format) {
			sox_close(_format);
		}
	}


	std::size_t format::read(sample_t* buf, std::size_t len) {
		return ::sox_read(_format, buf, len);
	}
}
