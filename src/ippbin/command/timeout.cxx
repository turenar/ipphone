#include "ippbin/command/timeout.hxx"

namespace ippbin {
	namespace command {
		void timeout_enable(terminal& t, terminal::command_vector&&) {
			t.enable_timeout();
		}

		void timeout_insert(terminal& t, terminal::command_vector&& args) {
			const std::string& name = args.front();
			if (name == "10yen") {
				t.println("inserted 10yen", t.color_pair_output);
				t.add_timeout(10);
			} else if (name == "100yen") {
				t.println("inserted 100yen", t.color_pair_output);
				t.add_timeout(100);
			} else if (name == "5000000000000000yen") {
				t.println("inserted 5000兆円", t.color_pair_output);
				t.add_timeout(5000'0000'0000'0000);
			}
		}
	}
}
