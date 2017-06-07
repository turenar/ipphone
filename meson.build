project('i1i2i3', 'cpp',
	default_options: ['cpp_std=c++14', 'warning_level=3'],
	version: '0.1.0',
	license: 'MIT')

cpp_compiler = meson.get_compiler('cpp')

if get_option('warning_level') == '3'
	#extra_warnings = ['-Wdelete-non-virtual-dtor', '-Wsuggest-final-methods', '-Wsuggest-final-types', '-Wsuggest-override', '-Wdisabled-optimization', '-Wfloat-equal', '-Wctor-dtor-privacy', '-Wlogical-op', '-Wnoexcept', '-Woverloaded-virtual', '-Wswitch-default', '-Wundef', '-Wold-style-cast']
	extra_warnings = []
	foreach warn : extra_warnings
		if cpp_compiler.has_argument(warn)
			add_project_arguments(warn, language: 'cpp')
		endif
	endforeach
endif

if get_option('buildtype').contains('debug')
	if cpp_compiler.has_argument('-rdynamic')
		add_project_link_arguments('-rdynamic', language: 'cpp')
	endif
endif


#configuration_data = 

g3logger_dep = dependency('g3logger', fallback: ['g3log', 'g3logger_dep'])

i1i2i3_files = [
	'src/main.cxx',
	'src/internal/log_console_sink.cxx',
]

executable('main', i1i2i3_files,
	dependencies: g3logger_dep)
