#pragma once

#include <atomic>
#include <cstdint>
#include <cstring>
#include <memory>
#include <condition_variable>

namespace ipp {
	namespace util {
		template <typename ElementType>
		class buffer {
		public:
			using element_type = ElementType;
			using array_type = element_type[];

			buffer(std::size_t buffer_size);

			std::size_t write(const element_type*, std::size_t len);
			std::size_t read(element_type*, std::size_t len);

			bool empty() const;
			bool full() const;

		protected:
			bool empty_without_locking() const;

			const std::size_t _buf_size;
			std::atomic<std::size_t> _read_ptr;
			std::atomic<std::size_t> _write_ptr;
			std::atomic<bool> _is_full;

			std::unique_ptr<array_type> _buf;
			mutable std::mutex _lock;
		};

		template <typename ElementType>
		inline
		buffer<ElementType>::buffer(std::size_t buffer_size)
				: _buf_size(buffer_size), _read_ptr(0), _write_ptr(0), _is_full(false) {
			_buf = std::make_unique<array_type>(buffer_size);
		}

		template <typename ElementType>
		inline std::size_t buffer<ElementType>::write(const element_type* elements, std::size_t len) {
			std::lock_guard<std::mutex> lock{_lock};

			std::size_t read_ptr = _read_ptr;
			std::size_t write_ptr = _write_ptr;
			if (_is_full || len == 0) {
				return 0; // fast return
			}
			const element_type* pointer = elements;
			std::size_t write_len = 0;
			std::size_t remain_len = len;
			LOG(DEBUG) << "write: " << write_len << ", remain: " << remain_len;
			if (write_ptr >= read_ptr && write_ptr + remain_len >= _buf_size) {
				std::size_t partial_len = _buf_size - write_ptr;
				std::memcpy(_buf.get() + write_ptr, elements, sizeof(element_type) * partial_len);
				write_ptr = 0;
				write_len = partial_len;
				pointer += partial_len;
				remain_len -= partial_len;
			}

			LOG(DEBUG) << "ptr: " << read_ptr << "," << write_ptr << ", write: " << write_len
			           << ", remain: " << remain_len;
			if (write_len > 0 && write_ptr == read_ptr) {
				_is_full = true;
			} else if (write_ptr < read_ptr && write_ptr + remain_len >= read_ptr) {
				// max writable will be read_ptr
				size_t partial_len = read_ptr - write_ptr;
				std::memcpy(_buf.get() + write_ptr, pointer, sizeof(element_type) * partial_len);
				write_len += partial_len;
				write_ptr += partial_len;
				_is_full = true;
			} else {
				std::memcpy(_buf.get() + write_ptr, pointer, sizeof(element_type) * remain_len);
				write_len += remain_len;
				write_ptr += write_len;
			}
			_write_ptr = write_ptr;
			return write_len;
		}

		template <typename ElementType>
		inline std::size_t buffer<ElementType>::read(element_type* elements, std::size_t len) {
			std::unique_lock<std::mutex> lock{_lock};

			std::size_t read_ptr = _read_ptr;
			std::size_t write_ptr = _write_ptr;
			if (len == 0 || (!_is_full && read_ptr == write_ptr)) {
				return 0; // fast return
			}
			element_type* pointer = elements;
			std::size_t read_len = 0;
			std::size_t remain_len = len;
			LOG(DEBUG) << "read: " << read_len << ", remain: " << remain_len;
			if (write_ptr <= read_ptr && read_ptr + remain_len >= _buf_size) {
				std::size_t partial_len = _buf_size - read_ptr;
				std::memcpy(pointer, _buf.get() + read_ptr, sizeof(element_type) * partial_len);
				read_ptr = 0;
				read_len = partial_len;
				pointer += partial_len;
				remain_len -= partial_len;
				_is_full = false;
			}

			LOG(DEBUG) << "ptr: " << read_ptr << "," << write_ptr << ", read: " << read_len
			           << ", remain: " << remain_len;
			if (!_is_full && read_ptr == write_ptr) {
				// empty
			} else if (read_ptr < write_ptr && read_ptr + remain_len > write_ptr) {
				size_t partial_len = write_ptr - read_ptr;
				std::memcpy(pointer, _buf.get() + read_ptr, sizeof(element_type) * partial_len);
				read_len += partial_len;
				read_ptr += partial_len;
			} else {
				std::memcpy(pointer, _buf.get() + read_ptr, sizeof(element_type) * remain_len);
				read_len += remain_len;
				read_ptr += remain_len;
			}
			_read_ptr = read_ptr;
			_is_full = false;
			return read_len;
		}

		template <typename ElementType>
		inline bool buffer<ElementType>::empty() const {
			std::lock_guard<std::mutex> lock{_lock};
			return empty_without_locking();
		}

		template <typename ElementType>
		inline bool buffer<ElementType>::empty_without_locking() const {
			return !_is_full && _read_ptr == _write_ptr;
		}

		template <typename ElementType>
		inline bool buffer<ElementType>::full() const {
			std::lock_guard<std::mutex> lock{_lock};
			return _is_full;
		}
	}
}
