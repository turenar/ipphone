sox_dep = dependency('sox')

sox_files = [
	'format.cxx',
]

libsoxcpp = library('soxcpp', sox_files,
	dependencies: sox_dep,
	include_directories: include_directories(['..']),
	cpp_args: project_warnings)
libsoxcpp_dep = declare_dependency(
	link_with: libsoxcpp,
	include_directories: include_directories(['..']))
