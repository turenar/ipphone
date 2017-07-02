#pragma once

#include <thread>
#include "ipp/device/handler.hxx"

namespace ipp {
	namespace device {
		class sox_read_handler : public handler {
		public:
			sox_read_handler();

			virtual ~sox_read_handler() {}

			bool shutting_down() const;
		private:
			std::thread _worker;
			std::atomic<bool> _shutting_down;
		};
	}
}
