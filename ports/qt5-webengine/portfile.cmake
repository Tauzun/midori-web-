###
##
##                          Welcome to QtWebEngine.
## https://chromium.googlesource.com/chromium/src/+/master/docs/windows_build_instructions.md
## https://bitbucket.org/chromiumembedded/cef/wiki/MasterBuildQuickStart.md#markdown-header-windows-setup
##
##                                  WARNING!
##
##  - REQUIRED: At least 8GB of RAM and 40GB of free disk space.
##  - Expect build times of between 4 & 6 hrs, depending upon the hardware specifications of the build machine. -
##
##  - This port file is experimental and requires 15-20 gigabytes of free space available. -
##  - The build dependencies of this piece of software are highly inflexible. -
##
###

vcpkg_check_linkage(ONLY_DYNAMIC_LIBRARY)
set(VCPKG_BUILD_TYPE release)
message(STATUS "${PORT} requires a lot of free disk space (>50GB), ram (>8GB) and time (>4h per configuration) to be successfully build.\n\
-- As such ${PORT} is EXPERIMENTAL.\n\
-- If ${PORT} fails post build validation please open up an issue. \n\
-- If it fails due to post validation the successfully installed files can be found in ${CURRENT_PACKAGES_DIR} \n\
-- and just need to be copied into ${CURRENT_INSTALLED_DIR}")
if(NOT VCPKG_TARGET_IS_WINDOWS)
    message(STATUS "If ${PORT} directly fails ${PORT} might require additional prerequisites on Linux and OSX. Please check the configure logs.\n")
endif()
include(${CURRENT_INSTALLED_DIR}/share/qt5/qt_port_functions.cmake)

vcpkg_find_acquire_program(FLEX)
vcpkg_find_acquire_program(BISON)
vcpkg_find_acquire_program(GPERF)
vcpkg_find_acquire_program(PYTHON2)
vcpkg_find_acquire_program(NINJA)
get_filename_component(FLEX_DIR "${FLEX}" DIRECTORY )
get_filename_component(BISON_DIR "${BISON}" DIRECTORY )
get_filename_component(PYTHON2_DIR "${PYTHON2}" DIRECTORY )
get_filename_component(GPERF_DIR "${GPERF}" DIRECTORY )
get_filename_component(NINJA_DIR "${NINJA}" DIRECTORY )

if(WIN32) # WIN32 HOST probably has win_flex and win_bison!
    if(NOT EXISTS "${FLEX_DIR}/flex${VCPKG_HOST_EXECUTABLE_SUFFIX}")
        file(CREATE_LINK "${FLEX}" "${FLEX_DIR}/flex${VCPKG_HOST_EXECUTABLE_SUFFIX}")
    endif()
    if(NOT EXISTS "${BISON_DIR}/BISON${VCPKG_HOST_EXECUTABLE_SUFFIX}")
        file(CREATE_LINK "${BISON}" "${BISON_DIR}/bison${VCPKG_HOST_EXECUTABLE_SUFFIX}")
    endif()
endif()

vcpkg_add_to_path(PREPEND "${FLEX_DIR}")
vcpkg_add_to_path(PREPEND "${BISON_DIR}")
vcpkg_add_to_path(PREPEND "${PYTHON2_DIR}")
vcpkg_add_to_path(PREPEND "${GPERF_DIR}")
vcpkg_add_to_path(PREPEND "${NINJA_DIR}")
## Timestamp announce to console
string(TIMESTAMP B_TS1 "Build started at %H:%M:%S on %d-%m-%y")
message(STATUS "* Build QtWebEngine from source begin:")
message(STATUS "* ${B_TS1}")
message(STATUS "* The process will take several hours")
message(STATUS "* (4-6 on average) to complete.")
message(STATUS "* Please be patient whilst the software is built")

set(PATCHES
        common.pri.patch
        gl.patch
        build_1.patch
        build_2.patch
        build_3.patch
)

if(NOT VCPKG_TARGET_IS_WINDOWS)
    set(OPTIONS
            "-webengine-system-libwebp"
            "-webengine-system-ffmpeg"
            "-webengine-system-icu"
            "-webengine-proprietary-codecs"
    )
else()
    set(OPTIONS
            "-webengine-proprietary-codecs"
    )
endif()

qt_submodule_installation(PATCHES ${PATCHES} BUILD_OPTIONS ${OPTIONS})

string(TIMESTAMP B_TS2 "Build completed at %H:%M:%S on %d-%m-%y")
message(STATUS "* ${B_TS2}")
