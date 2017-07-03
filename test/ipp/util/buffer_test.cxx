#include <type_traits>
#include <g3log/logworker.hpp>
#include <gtest/gtest.h>
#include "ipp/logger/log_console_sink.hxx"
#include "ipp/util/buffer.hxx"

namespace ipp {
	namespace util {
		namespace {
			namespace buffer_test {
				template <typename ElementType, typename Array>
				void test_write(std::size_t expected, buffer<ElementType>& buf, const Array& w,
				                std::size_t begin, std::size_t len) {
					ASSERT_EQ(expected, buf.write(w + begin, len));
				}

				template <typename ElementType, typename Array>
				void test_read(std::size_t expected, buffer<ElementType>& buf, const Array& w, Array& r,
				               std::size_t begin, std::size_t len) {
					ASSERT_EQ(expected, buf.read(r, len));
					std::basic_string<ElementType> wstr(w + begin, expected);
					std::basic_string<ElementType> rstr(r, expected);
					ASSERT_EQ(wstr, rstr);
				}

				template <typename ElementType>
				void test_generic() {
#define TEST_WRAP(expr) do { SCOPED_TRACE(#expr); expr; if (::testing::Test::HasFatalFailure()) { return; }} while(false)
					buffer<ElementType> buf(4);
					const ElementType w[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
					ElementType r[std::extent<decltype(w)>::value] = {};
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

					TEST_WRAP(test_write(4, buf, w, 0, 4));
					TEST_WRAP(test_read(4, buf, w, r, 0, 4));
					TEST_WRAP(test_write(4, buf, w, 0, 4));
					TEST_WRAP(test_read(4, buf, w, r, 0, 4));
					TEST_WRAP(test_write(4, buf, w, 0, 4));
					TEST_WRAP(test_read(4, buf, w, r, 0, 4));
					TEST_WRAP(test_write(4, buf, w, 0, 4));
					TEST_WRAP(test_read(4, buf, w, r, 0, 4));
					TEST_WRAP(test_write(4, buf, w, 0, 4));
					TEST_WRAP(test_read(4, buf, w, r, 0, 4));
					TEST_WRAP(test_write(4, buf, w, 0, 4));
					TEST_WRAP(test_read(4, buf, w, r, 0, 4));
				}
			}
		}

		TEST(BufferTest, SingleByte) {
			SCOPED_TRACE("test_generic<char>");
			buffer_test::test_generic<char>();
		}

		TEST(BufferTest, MultiByte) {
			SCOPED_TRACE("test_generic<short>");
			buffer_test::test_generic<char16_t>();
		}
	}
}
