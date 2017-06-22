#!/bin/sh

if ${BUILD_OPTIONS+:} false; then
	:
else
	echo "Specify BUILD_OPTIONS!" >&2
	exit 1
fi

mkdir build
cd build
meson .. ${BUILD_OPTIONS}
ninja -j1
