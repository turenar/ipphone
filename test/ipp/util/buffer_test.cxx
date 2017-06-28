#include <gtest/gtest.h>
#include <g3log/logworker.hpp>
#include "ipp/logger/log_console_sink.hxx"
#include "ipp/util/buffer.hxx"

namespace ipp {
	namespace util {
		namespace {
			template <typename Buffer, typename Array>
			void test_write(std::size_t expected, Buffer& buf, const Array& w, std::size_t begin, std::size_t len) {
				ASSERT_EQ(expected, buf.write(w + begin, len));
			}

			template <typename Buffer, typename Array>
			void test_read(std::size_t expected, Buffer& buf, const Array& w, Array& r,
			               std::size_t begin, std::size_t len) {
				ASSERT_EQ(expected, buf.read(r, len));
				std::string wstr(w + begin, expected);
				std::string rstr(r, expected);
				ASSERT_EQ(wstr, rstr);
			}
		}

		TEST(BufferTest, SingleByte) {
#define TEST_WRAP(expr) do { SCOPED_TRACE(#expr); expr; if (HasFatalFailure()) { return; }} while(false)
			auto worker = g3::LogWorker::createLogWorker();
			worker->addSink(std::make_unique<ipp::logger::log_console_sink>(), &ipp::logger::log_console_sink::log);
			g3::initializeLogging(worker.get());
			
			buffer<char> buf(4);
			const char w[] = "1234567890";
			char r[sizeof(w)] = {};
			ASSERT_TRUE(buf.empty());
			ASSERT_FALSE(buf.full());
			TEST_WRAP(test_write(1, buf, w, 0, 1));
			ASSERT_FALSE(buf.empty());
			TEST_WRAP(test_write(2, buf, w, 1, 2));
			ASSERT_FALSE(buf.full());
			TEST_WRAP(test_write(1, buf, w, 3, 3));
			ASSERT_TRUE(buf.full());
			TEST_WRAP(test_write(0, buf, w, 6, 4));
			ASSERT_TRUE(buf.full());
			TEST_WRAP(test_read(1, buf, w, r, 0, 1));
			ASSERT_FALSE(buf.full());
			TEST_WRAP(test_read(2, buf, w, r, 1, 2));
			TEST_WRAP(test_read(1, buf, w, r, 3, 3));
			ASSERT_TRUE(buf.empty());
			// this should block...
			// TEST_WRAP(test_read(1, buf, w, r, 0, 1));
			TEST_WRAP(test_write(4, buf, w, 0, 4));
			TEST_WRAP(test_read(3, buf, w, r, 0, 3));
			TEST_WRAP(test_write(3, buf, w, 4, 4));
			TEST_WRAP(test_read(4, buf, w, r, 3, 4));
		}
	}
}
