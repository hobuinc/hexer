#!/bin/bash
#
CONFIG="Unix Makefiles"
BUILD_TYPE="Release"
if ! [ -z "$1" ]; then
    CONFIG="$1"
fi

rm -rf build
mkdir -p build && cd build

cmake ..  -G "$CONFIG"  \
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
        -DCMAKE_INSTALL_PREFIX=${CONDA_PREFIX} \
        -DWITH_DRAWING=ON
