boost_dep = dependency('boost', modules : ['exception'])
g3logger_dep = dependency('g3logger', fallback: ['g3log', 'g3logger_dep'])

i1i2i3_files = [
	'main.cxx',
	'error_info.cxx',
	'network/socket.cxx',
	'network/socket_connection.cxx',
	'network/socket_exception.cxx',
	'internal/log_console_sink.cxx',
]

vcs_rev = vcs_tag(input: 'version.hxx.in', output: 'version.hxx',
	fallback: meson.project_version())

executable('main', [i1i2i3_files, vcs_rev],
	dependencies: [boost_dep, g3logger_dep],
	include_directories: include_directories(['.']))
