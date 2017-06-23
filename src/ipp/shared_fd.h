#pragma once

#include <memory>
#include "ipp/unique_fd.h"

namespace ipp {
	class shared_fd {
	public:
		using fd_type = unique_fd::fd_type;
		static constexpr fd_type invalid_fd = unique_fd::invalid_fd;

		shared_fd();
		explicit shared_fd(int);
		shared_fd(const shared_fd&);
		shared_fd(shared_fd&&);
		shared_fd(unique_fd&&);

		shared_fd& operator=(const shared_fd&);
		shared_fd& operator=(shared_fd&&);
		shared_fd& operator=(unique_fd&&);

		~shared_fd() = default;

		operator fd_type();
		fd_type get();
		explicit operator bool();

	private:
		fd_type _fd;
		std::shared_ptr<unique_fd> _ptr;
	};

	inline shared_fd::shared_fd() : _fd(unique_fd::invalid_fd) {}

	inline shared_fd::shared_fd(int fd) : shared_fd(unique_fd(fd)) {}

	inline shared_fd::shared_fd(const shared_fd& fd) { *this = fd; }

	inline shared_fd::shared_fd(shared_fd&& fd) { *this = std::move(fd); }

	inline shared_fd::shared_fd(unique_fd&& fd) { *this = std::move(fd); }


	inline shared_fd& shared_fd::operator=(const shared_fd& another) {
		_ptr = another._ptr;
		_fd = another._fd;
		return *this;
	}

	inline shared_fd& shared_fd::operator=(shared_fd&& other) {
		_ptr = std::move(other._ptr);
		_fd = other._fd;
		other._fd = invalid_fd;
		return *this;
	}

	inline shared_fd& shared_fd::operator=(unique_fd&& other) {
		_fd = other.get();
		_ptr = std::make_shared<unique_fd>(std::move(other));
		return *this;
	}

	inline shared_fd::operator fd_type() {
		return _fd;
	}

	inline shared_fd::fd_type shared_fd::get() { return _fd; }

	inline shared_fd::operator bool() { return _fd >= 0; }
}
