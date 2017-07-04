#pragma once

#include <chrono>
#include <utility>
#include <unistd.h>
#include "ipp/ipp_system_exception.hxx"

#ifndef NDEBUG
#include "ipp/logger/logger.hxx"
#endif

namespace ipp {
	class unique_fd {
	public:
		using fd_type = int;
		static constexpr fd_type invalid_fd = -1;

		unique_fd();
		explicit unique_fd(int);
		unique_fd(const unique_fd&) = delete;
		unique_fd(unique_fd&&) noexcept;
		unique_fd& operator=(const unique_fd&) = delete;
		unique_fd& operator=(unique_fd&&) noexcept;

		~unique_fd();

		explicit operator fd_type();
		fd_type get();
		explicit operator bool() const;

		bool readable(std::chrono::milliseconds timeout) const;

	private:
		fd_type _fd;
	};

	inline unique_fd::unique_fd() : _fd(invalid_fd) {}

	inline unique_fd::unique_fd(int fd) : _fd(fd) {}

	inline unique_fd::unique_fd(unique_fd&& fd) noexcept { *this = std::move(fd); }

	inline unique_fd& unique_fd::operator=(unique_fd&& another) noexcept {
		_fd = another._fd;
		another._fd = invalid_fd;
		return *this;
	}

	inline unique_fd::~unique_fd() {
		if (*this) {
#ifndef NDEBUG
			LOG(DEBUG) << "closing fd: " << _fd;
#endif
			int result = close(_fd);
#ifndef NDEBUG
			if (result < 0) {
				LOG(ERROR) << "failed closing fd: " << _fd << ", result: " << result;
			}
#endif
			(void) result;
		}
	}

	inline unique_fd::operator fd_type() { return _fd; }

	inline unique_fd::fd_type unique_fd::get() { return _fd; }

	inline unique_fd::operator bool() const { return _fd >= 0; }

	inline bool unique_fd::readable(std::chrono::milliseconds timeout) const {
		fd_set rfds;
		FD_ZERO(&rfds);
		FD_SET(_fd, &rfds);

		timeval tv;
		auto timeout_sec = std::chrono::duration_cast<std::chrono::seconds>(timeout);
		tv.tv_sec = timeout_sec.count();
		tv.tv_usec = (timeout - timeout_sec).count();
		int result = ::select(_fd + 1, &rfds, NULL, NULL, &tv);
		if (result < 0) {
			IPP_THROW_EXCEPTION(ipp_system_exception(errno, std::system_category()));
		}
		return result > 0;
	}
}
