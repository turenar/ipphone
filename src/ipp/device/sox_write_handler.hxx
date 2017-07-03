#pragma once

#include <thread>
#include "ipp/device/handler.hxx"

namespace ipp {
	namespace device {
		class sox_write_handler : public handler {
		public:
			sox_write_handler();

			virtual ~sox_write_handler();

			void shutdown();
			bool shutting_down() const;
		private:
			std::thread _worker;
			std::atomic<bool> _shutting_down;
		};
	}
}
