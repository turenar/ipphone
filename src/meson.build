conf_data = configuration_data()
has_send_packet = cpp_compiler.has_function('avcodec_send_packet',
	dependencies: libavcodec_dep)
conf_data.set('IPPBIN_HAVE_AVCODEC_SEND_PACKET', has_send_packet)
conf_file = configure_file(output: 'config.h', configuration: conf_data)

subdir('sox')
subdir('ipp')

main_src = [
	'ippbin/command/phone.cxx',
	'ippbin/command/timeout.cxx',
	'ippbin/logger.cxx',
	'ippbin/sixel_animation.cxx',
	'ippbin/terminal.cxx',
	conf_file,
]

executable('ippbin', ['ippbin.cxx'],
	dependencies: [libipp_dep, ncurses_dep, libsixel_dep],
	cpp_args: project_warnings)
executable('main', ['main.cxx', main_src],
	dependencies: [libipp_dep, ncurses_dep, libsixel_dep],
	cpp_args: project_warnings)
