#!/bin/bash

set -euo pipefail

: "${CONDA_PREFIX:?Activate the hexer conda environment before running hobu.sh.}"

rm -rf build
cmake -S . -B build -G Ninja \
    -DWITH_TESTS=ON \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX="${CONDA_PREFIX}"
cmake --build build
