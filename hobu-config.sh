USR_LOCAL="/usr/local"
USR="/usr"
SO_EXT=dylib
EMBED=ON

CONFIG="Unix Makefiles"
BUILD_TYPE="Debug"
if ! [ -z "$1" ]; then
    CONFIG="$1"
fi

cmake   -G "$CONFIG"  \
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
        -DCMAKE_INSTALL_PREFIX=/Users/hobu \
        -DWITH_GDAL=ON \
        -DWITH_DRAWING=ON 
