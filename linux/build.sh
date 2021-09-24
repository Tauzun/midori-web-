#!/bin/bash
#
#
# Builds a binary of the application
#

# See the file 'Dockerfile' for the dnf command to install the build deps when using Fedora (version 33 or newer recommended)

# Set NO_PLUGINS:BOOL="0" to build the plugins

CURDIR="${PWD}"
BINOUT="$CURDIR/../build/midori"

cd ..
if [ -d "build" ]; then rm -rf build; fi
mkdir build && cd build
# CMAKE_CONFIGURATION_TYPES are "Release" and "Debug"
cmake -DCMAKE_CONFIGURATION_TYPES:STRING="Release" \
-DBUILD_TESTING:BOOL="0" \
-DNO_PLUGINS:BOOL="1" \
-DCMAKE_INSTALL_PREFIX=$BINOUT \
-DBUILD_SHARED_LIBS:BOOL="1" ..
make >> build_log.txt && make install >> install_log.txt
if [ -f midori/bin/midori ]; then ./midori/bin/midori; fi
cd ..
cd linux

exit
