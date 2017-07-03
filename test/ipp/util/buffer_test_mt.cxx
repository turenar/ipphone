#include <type_traits>
#include <g3log/logworker.hpp>
#include <gtest/gtest.h>
#include "ipp/logger/log_console_sink.hxx"
#include "ipp/util/buffer.hxx"

namespace ipp {
	namespace util {
		namespace {
			namespace buffer_test_mt {
				constexpr int loop_count = 100000;

				template <typename ElementType>
				void test_generic() {
					auto worker = g3::LogWorker::createLogWorker();
					worker->addSink(std::make_unique<ipp::logger::log_console_sink>(),
					                &ipp::logger::log_console_sink::log);
					g3::initializeLogging(worker.get());

					buffer<ElementType> buf(4);
					std::mutex m;
					std::condition_variable cv;
					std::atomic<bool> ready(false);
					std::atomic<bool> write_finished(false);
					std::atomic<int> read_count(0);
					std::atomic<int> write_count(0);

					std::thread a([&m, &cv, &ready, &buf, &write_count, &write_finished] {
						{
							std::unique_lock<std::mutex> lock(m);
							cv.wait(lock, [&ready]() -> bool { return ready; });
						}
						LOG(DEBUG) << "wake up writer";
						int wc = 0;
						const ElementType dat[] = {0, 1, 2, 3};
						for (int i = 0; i < loop_count; i++) {
							wc += buf.write(dat, std::extent<decltype(dat)>::value);
						}
						write_count = wc;
						write_finished = true;
						{
							std::lock_guard<std::mutex> lock(m);
							cv.notify_one();
						}
					});
					std::thread b([&m, &cv, &ready, &buf, &read_count, &write_finished] {
						{
							std::unique_lock<std::mutex> lock(m);
							cv.wait(lock, [&ready]() -> bool { return ready; });
						}
						LOG(DEBUG) << "wake up reader";
						int rc = 0;

						ElementType dat[4];
						for (int i = 0; i < loop_count; i++) {
							rc += buf.read(dat, std::extent<decltype(dat)>::value);
						}

						{
							std::unique_lock<std::mutex> lock(m);
							cv.wait(lock, [&write_finished]() -> bool { return write_finished; });
						}
						rc += buf.read(dat, std::extent<decltype(dat)>::value);
						read_count = rc;
					});
					{
						std::lock_guard<std::mutex> lock(m);
						ready = true;
						cv.notify_all();
					}
					if (a.joinable()) {
						a.join();
					}
					b.join();
					ASSERT_EQ(write_count, read_count);
				}
			}
		}

		TEST(BufferTestMT, SingleByte) {
			SCOPED_TRACE("test_generic<char>");
			buffer_test_mt::test_generic<char>();
		}

		TEST(BufferTestMT, MultiByte) {
			SCOPED_TRACE("test_generic<short>");
			buffer_test_mt::test_generic<char16_t>();
		}
	}
}
