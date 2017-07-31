project('i1i2i3', 'cpp',
	default_options: [
		'cpp_std=c++14',
		'warning_level=3',
		'default_library=static',
	],
	version: '0.1.0',
	license: 'MIT')

cpp_compiler = meson.get_compiler('cpp')

project_warnings = []
if get_option('warning_level') == '3'
	extra_warnings = [
		'-Wctor-dtor-privacy',
		'-Wdelete-non-virtual-dtor',
		'-Wdisabled-optimization',
		'-Wfloat-equal',
		'-Wlogical-op',
		'-Wnoexcept',
		'-Wold-style-cast',
		'-Woverloaded-virtual',
		'-Wsuggest-final-methods',
		'-Wsuggest-final-types',
		'-Wsuggest-override',
		'-Wswitch-default',
		'-Wundef',
	]
	foreach warn : extra_warnings
		if cpp_compiler.has_argument(warn)
			# add_project_arguments(warn, language: 'cpp')
			project_warnings += [warn]
		endif
	endforeach
endif

if get_option('buildtype').contains('debug')
	if cpp_compiler.has_argument('-rdynamic')
		add_project_link_arguments('-rdynamic', language: 'cpp')
	endif
endif

boost_dep = dependency('boost', modules : ['exception'])
ncurses_dep = dependency('ncursesw')
g3logger_dep = dependency('g3logger',
    fallback: ['g3log', 'g3logger_dep'],
    default_options: ['dynamic_logging=true']
)

libavcodec_dep = dependency('libavcodec')
libavutil_dep = dependency('libavutil')
libswscale_dep = dependency('libswscale')
ffmpeg_dep = declare_dependency(
	dependencies: [libavcodec_dep, libavutil_dep, libswscale_dep])
libsixel_dep = dependency('libsixel')

conf = configuration_data()

subdir('src')
subdir('test')
