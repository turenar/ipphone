subdir('sox')
subdir('ipp')

main_src = [
	'ippbin/terminal.cxx',
	'ippbin/logger.cxx',
]
executable('ippbin', ['ippbin.cxx', main_src],
	dependencies: [libipp_dep, ncurses_dep],
	cpp_args: project_warnings)
executable('main', ['main.cxx', main_src],
	dependencies: [libipp_dep, ncurses_dep],
	cpp_args: project_warnings)
