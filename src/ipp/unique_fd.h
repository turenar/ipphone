#pragma once

#include <utility>
#include <unistd.h>
#include "ipp/logger/logger.hxx"

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

		operator fd_type();
		fd_type get();
		explicit operator bool();

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
			LOG(DEBUG) << "closing fd: " << _fd;
			close(_fd);
		}
	}

	inline unique_fd::operator fd_type() { return _fd; }

	inline unique_fd::fd_type unique_fd::get() { return _fd; }

	inline unique_fd::operator bool() { return _fd >= 0; }
}
