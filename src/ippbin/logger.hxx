#pragma once

#include <memory>

namespace g3 { class LogWorker; }

namespace ippbin {
	class logger {
	public:
		logger();
		~logger();
		void install_logger();

	private:
		std::unique_ptr<g3::LogWorker> _worker;
	};
}
