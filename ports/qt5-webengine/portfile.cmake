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

set(VCPKG_BUILD_TYPE release) # By default, we only build release currently due to how large QtWebEngine is. Please comment out this set command if you dare to try build debug
vcpkg_check_linkage(ONLY_DYNAMIC_LIBRARY)
message(STATUS "${PORT} requires a lot of free disk space (>300GB), lots of RAM (16GB or more has been tested) and time (>4h per configuration) to be successfully build.\n\
-- As such ${PORT} may not build at all on your system.\n\
-- If ${PORT} fails post build validation please ensure you're using the latest portfiles, then submit an issue ticket at https://github.com/Microsoft/vcpkg/issues \n\
-- If it fails due to post validation the successfully installed files can be found in <vcpkgroot>/packages/${PORT}_${TARGET_TRIPLET} and just need to be copied into <vcpkgroot>/installed/${TARGaET_TRIPLET}")
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
get_filename_component(NINJA_DIR "${NINJA}" DIRECTORY ) # QtWebEngine ships with a version of ninja, comment this line out if you want to use it

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
vcpkg_add_to_path(PREPEND "${NINJA_DIR}") # QtWebEngine ships with a version of ninja, comment this line out if you want to use it

## Timestamp announce to console
string(TIMESTAMP B_TS1 "Build started at %H:%M:%S on %d-%m-%y")
message(STATUS "* Build QtWebEngine from source begin:")
message(STATUS "* ${B_TS1}")
message(STATUS "* The process will take several hours")
message(STATUS "* (4-6 on average) to complete.")
message(STATUS "* Please be patient whilst the software is built")

if(QT_MAJOR_MINOR_VER STREQUAL "5.14")

    qt_download_submodule(OUT_SOURCE_PATH TARGET_SOURCE_PATH ${ARGV})

    if(EXISTS ${TARGET_SOURCE_PATH}/src/3rdparty/chromium/README.md)

        message(STATUS "* Patching the source tree...")
        ### ------------------------------------------------------
        ##                  "Fixes"
        ### ------------------------------------------------------

        ## Some changes to /src/buildtools/config/common.pri
        file(READ ${TARGET_SOURCE_PATH}/src/buildtools/config/common.pri COM_PRI)
        ## "Fix" the "Ninja does not see existing precompiled header" error https://bugreports.qt.io/browse/QTBUG-65677
        ## Compiling the headers means the build takes significantly longer to complete
        string(REPLACE "gn_args += enable_precompiled_headers=true" "gn_args += enable_precompiled_headers=false" COM_PRI "${COM_PRI}")
        ##
        ## Enable proprietary codecs
        string(REPLACE "qtConfig(webengine-proprietary-codecs):" " " COM_PRI "${COM_PRI}")
        ##
        ## Write out the file
        file(WRITE ${TARGET_SOURCE_PATH}/src/buildtools/config/common.pri "${COM_PRI}")
        ## "Fix" the "chromium\ui\gl\gl_bindings_autogen_gl.h(401): error C2061: syntax error: identifier 'GLeglImageOES'" error message
        ## when compiling without precompiled headers
        file(READ ${TARGET_SOURCE_PATH}/src/3rdparty/chromium/ui/gl/gl_bindings_autogen_gl.h GLB_H)
	    string(REPLACE "class GLContext;" "class GLContext;\n\ntypedef void *GLeglImageOES;" GLB_H "${GLB_H}")
	    file(WRITE ${TARGET_SOURCE_PATH}/src/3rdparty/chromium/ui/gl/gl_bindings_autogen_gl.h "${GLB_H}")
        file(REMOVE_RECURSE ${TARGET_SOURCE_PATH}/src/3rdparty/ninja) # QtWebEngine ships with a version of ninja, comment this line out if you want to use it

        ### ------------------------------------------------------

        ## Prevent patching twice upon rebuilds
        file(REMOVE ${TARGET_SOURCE_PATH}/src/3rdparty/chromium/README.md)

        message(STATUS "* Source tree patched")

    else()

        message(STATUS "* Source tree is already patched...")

    endif()
    ## Add some includes to the INCLUDEPATH
    set(ENV{INCLUDEPATH} "${TARGET_SOURCE_PATH}/include/;$ENV{INCLUDEPATH}")
    #set(ENV{PATH} "${TARGET_SOURCE_PATH}/src/3rdparty/ninja/;$ENV{PATH}") # QtWebEngine ships with a version of ninja, uncomment this line if you want to use it
    set(ENV{PATH} "${TARGET_SOURCE_PATH}/src/3rdparty/gn/);$ENV{PATH}")

    if(QT_UPDATE_VERSION)
        SET(VCPKG_POLICY_EMPTY_PACKAGE enabled PARENT_SCOPE)
    else()
        qt_build_submodule(${TARGET_SOURCE_PATH} ${ARGV})
        qt_install_copyright(${TARGET_SOURCE_PATH})
    endif()

else()

qt_submodule_installation(PATCHES 
                                common.pri.patch
                                gl.patch)

endif()

string(TIMESTAMP B_TS2 "Build completed at %H:%M:%S on %d-%m-%y")
message(STATUS "* ${B_TS2}")
