#!/bin/bash

function _die() {
	test "$no_parallel" != 1 && cat $1 >&3
	exit 1
}

function check_compile1() {
	test -d bin/$1 && rm -rf bin/$1
	mkdir bin/$1
	pushd bin/$1 >/dev/null 2>&1
	shift
	if [ "$no_parallel" != 1 ]; then
		NINJAOPTS="${NINJAOPTS-}${NINJAOPTS+ }-j1"
		exec 3>&1
		exec >cibuild.log
	fi
	meson ../.. ${CONF_OPTS:-} "$@" 2>&1 || _die cibuild.log
	ninja ${NINJAOPTS} 2>&1 || _die cibuild.log
	#if ! ninja check 2>&1; then
	#	if [ "$no_parallel" = 1 ]; then
	#		test_failed=1
	#	else
	#		cat cibuild.log >&3
	#		exit 1
	#	fi
	#fi
	popd >/dev/null 2>&1
	if [ "$no_parallel" != 1 ]; then
		exec >&3
	fi
}

function run_job() {
	if [ "$no_parallel" = 0 ]; then
		echo "CXX=${CXX}" ./cibuild.sh -c "$@" >&9
	else
		echo "$@..."
		check_compile1 "$@"
	fi
}

function check_compile() {
	local name
	name="$1"
	shift
	run_job "${name}-debug" "$@"
	run_job "${name}-release" "$@" -Dbuildtype=release -Db_ndebug=true
	run_job "${name}-relunity" "$@" -Dbuildtype=release -Db_ndebug=true -Dunity=on
	if [ ${CXX} = g++ ]; then
		run_job "${name}-rellto" "$@" -Dbuildtype=release -Db_ndebug=true -Db_lto=true
	fi
}

function has_cmd() {
	which $1 >/dev/null 2>&1
}

if test "$1" = "-c"; then
	shift
	check_compile1 "$@"
	exit $?
fi

cd "$(dirname "$0")"

if [ ${no_parallel:-0} = 0 ]; then
	if which parallel >/dev/null 2>&1; then
		echo 'using parallel'
		test -e .cibuild.pipe && rm .cibuild.pipe
		mkfifo .cibuild.pipe
		parallel --halt=1 --progress -v <.cibuild.pipe &
		parallel_pid=$!
		exec 9>.cibuild.pipe
		rm .cibuild.pipe
		no_parallel=0
	else
		no_parallel=1
	fi
fi
export no_parallel
test_failed=0

#./autogen.sh -v
test -d bin || mkdir bin

#has_cmd x86_64-w64-mingw32-gcc && check_compile cibuild-mingw-x86_64 --host=x86_64-w64-mingw32
#has_cmd i686-w64-mingw32-gcc && check_compile cibuild-mingw-i686 --host=i686-w64-mingw32
CXX=g++ check_compile cibuild-native
CXX=clang++ check_compile cibuild-native+c

if ${parallel_pid+:} false; then
	exec 9>&-
	wait ${parallel_pid} || exit $?
fi

if false && which gcovr >/dev/null 2>&1; then
	check_compile1 cibuild-coverage --enable-debug --enable-coverage
	cd bin/cibuild-coverage/src
	gcovr --xml --output=coverage.xml
	cd ../../..
	proj_dir="$(pwd)"
	dot_dir="$(echo "${proj_dir}" | sed -e 's|/|.|g')"
	sed -i.bak -e "s@${proj_dir}/@@g" -e "s@${dot_dir}@@g" bin/cibuild-coverage/src/coverage.xml
fi

if [ $test_failed -ne 0 ]; then
	echo "there is failed tests!" >&2
	exit 201
else
	exit 0
fi
