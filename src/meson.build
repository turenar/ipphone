subdir('sox')
subdir('ipp')

executable('ippbin', ['ippbin.cxx', vcs_rev],
	dependencies: libipp_dep,
	cpp_args: project_warnings)
executable('main', 'main.cxx')
