subdir('sox')
subdir('ipp')

main_src = [
	'ippbin/command/phone.cxx',
	'ippbin/command/timeout.cxx',
	'ippbin/logger.cxx',
	'ippbin/sixel_animation.cxx',
	'ippbin/terminal.cxx',
]
executable('ippbin', ['ippbin.cxx', main_src],
	dependencies: [libipp_dep, ncurses_dep, libsixel_dep],
	cpp_args: project_warnings)
executable('main', ['main.cxx', main_src],
	dependencies: [libipp_dep, ncurses_dep, libsixel_dep],
	cpp_args: project_warnings)
