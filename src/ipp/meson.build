boost_dep = dependency('boost', modules : ['exception'])
g3logger_dep = dependency('g3logger',
	fallback: ['g3log', 'g3logger_dep'],
	default_options: ['dynamic_logging=true']
)

i1i2i3_files = [
	'error_info.cxx',
	'logger/log_console_sink.cxx',
	'main.cxx',
	'network/socket.cxx',
	'network/socket_address.cxx',
	'network/socket_connection.cxx',
	'protocol/connection.cxx',
	'protocol/connection_manager.cxx',
	'protocol/ippp.cxx',
	'protocol/packet/packet_builder.cxx',
]

vcs_rev = vcs_tag(input: 'version.hxx.in', output: 'version.hxx',
	fallback: meson.project_version())

executable('main', [i1i2i3_files, vcs_rev],
	dependencies: [boost_dep, g3logger_dep],
	include_directories: include_directories(['..']))
