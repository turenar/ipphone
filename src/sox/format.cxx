#include "sox/format.hxx"
#include "ipp/ipp_exception.hxx"

namespace sox {
	format format::open_device_for_read(const std::string& path, const signalinfo* signal, const encodinginfo* encoding,
	                                    const std::string& filetype) {
		sox_format_t* form = ::sox_open_read(path.c_str(), signal, encoding, filetype.c_str());
		if (form == nullptr) {
			IPP_THROW_EXCEPTION(ipp::ipp_exception("sox_open_read returned null"));
		}
		return format(form);
	}

	format format::open_device_for_write(const std::string& path, const signalinfo* signal, const encodinginfo* encoding,
	                                     const std::string& filetype, const oob* oob) {
		sox_format_t* form = ::sox_open_write(path.c_str(), signal, encoding, filetype.c_str(), oob, NULL);
		if (form == nullptr) {
			IPP_THROW_EXCEPTION(ipp::ipp_exception("sox_open_write returned null"));
		}
		return format(form);
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

	std::size_t format::write(const sample_t* buf, std::size_t len) {
		return ::sox_write(_format, buf, len);
	}
}
