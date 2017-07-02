#pragma once

#include "ipp/device/handler.hxx"
#include <thread>

namespace ipp {
	namespace device {
		class sox_write_handler : public handler {
		public:
			sox_write_handler();

			virtual ~sox_write_handler() {}

			bool shutting_down() const;
		private:
			std::thread _worker;
			std::atomic<bool> _shutting_down;
		};
	}
}
