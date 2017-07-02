#pragma once

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <cstring>
#include <memory>

namespace ipp {
	namespace util {
		template <typename ElementType>
		class buffer {
		public:
			using element_type = ElementType;
			using array_type = element_type[];
			using timeout_type = std::chrono::milliseconds;

			buffer(std::size_t buffer_size);

			std::size_t write(const element_type*, std::size_t len);
			std::size_t write(const element_type*, std::size_t len, timeout_type timeout);
			std::size_t read(element_type*, std::size_t len);
			std::size_t read(element_type*, std::size_t len, timeout_type timeout);

			std::size_t buffer_size() const;
			std::size_t buffered() const;
			bool empty() const;
			bool full() const;

		protected:
			template <typename WaitCondition>
			std::size_t write(const element_type*, std::size_t len, WaitCondition);
			template <typename WaitCondition>
			std::size_t read(element_type*, std::size_t len, WaitCondition);

			bool empty_without_lock() const;
			bool full_without_lock() const;

			const std::size_t _buf_size;
			std::atomic<std::size_t> _read_ptr;
			std::atomic<std::size_t> _write_ptr;
			std::atomic<bool> _is_full;

			std::unique_ptr<array_type> _buf;
			mutable std::mutex _lock;
			std::condition_variable _cv;
		};
	}
}

#include "detail/buffer.hxx"
