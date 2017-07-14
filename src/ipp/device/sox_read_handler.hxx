#pragma once

#include <thread>
#include "ipp/device/handler.hxx"

namespace ipp {
	namespace device {
		class sox_read_handler final : public handler {
		public:
			sox_read_handler();

			virtual ~sox_read_handler();

			void shutdown();
			bool shutting_down() const;
		private:
			std::thread _worker;
			std::atomic<bool> _shutting_down;
		};
	}
}
