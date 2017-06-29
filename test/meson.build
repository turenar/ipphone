gtest = subproject('gtest')
gtest_libsrc = gtest.get_variable('libsources')
gtest_mainsrc = gtest.get_variable('mainsources')
gtest_inc = gtest.get_variable('incdir')

test_sources = [
	'ipp/util/buffer_test.cxx',
	'ipp/util/buffer_test_mt.cxx',
]

gtest_lib = static_library('gtest', [gtest_libsrc],
	include_directories: gtest_inc,
	cpp_args: ['-w'])

ipptest = executable('ipptest', [gtest_mainsrc, test_sources],
	link_with: gtest_lib,
	include_directories: [include_directories('.'), gtest_inc],
	dependencies: libipp_dep)
test('ipptest', ipptest)
