cmake_minimum_required(VERSION 3.1)
project(Midori VERSION 10.0.1)
# CMake version must be the very first line
set(CMAKE_CXX_STANDARD 14) # Enable C++14
set(CMAKE_CXX_EXTENSIONS OFF) # Don't enable gcc-specific extensions

# Find ECM, with nice error handling in case of failure

include(FeatureSummary)
find_package(ECM 5.27.0 CONFIG)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://commits.kde.org/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if (NOT DEBUG)
    set(UNITY_BUILD true)
endif()

# Many includes from ECM, to get all the nice cmake functions and settings

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMInstallIcons)
include(ECMSetupVersion)
include(ECMAddAppIcon)
include(ECMQtDeclareLoggingCategory)
include(ECMPoQmTools)

# Output dirs (like ECM 5.38 does)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if (UNIX AND NOT APPLE)
    set(MIDORI_INSTALL_PLUGINDIR "${KDE_INSTALL_PLUGINDIR}/midori")
else()
    set(MIDORI_INSTALL_PLUGINDIR "${KDE_INSTALL_PLUGINDIR}")
endif()

if (IS_ABSOLUTE ${MIDORI_INSTALL_PLUGINDIR})
    set(PLUGIN_PATH "${MIDORI_INSTALL_PLUGINDIR}")
else()
    set(PLUGIN_PATH "${CMAKE_INSTALL_PREFIX}/${MIDORI_INSTALL_PLUGINDIR}")
endif()

if (NOT WIN32)
    set(MIDORI_PLUGIN_PATH "${PLUGIN_PATH}" CACHE PATH "Default plugin search path")
endif()

# Defines that are always set

add_definitions(-DQT_NO_URL_CAST_FROM_STRING -DQT_USE_QSTRINGBUILDER -DQT_NO_CAST_TO_ASCII)

# Mandatory: Qt5

set(QT_MIN_VERSION "5.14.2")
if (WIN32)
    find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS Core Widgets Network Sql QuickWidgets PrintSupport WebEngine WebEngineWidgets WebChannel)
else()
    find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS Core Widgets Network Sql QuickWidgets PrintSupport WebChannel)
    find_package(Qt5WebEngine ${QT_MIN_VERSION} REQUIRED) # Workaround in case of unusual location on Linux (Flatpack)
    find_package(Qt5WebEngineWidgets ${QT_MIN_VERSION} REQUIRED) # Workaround in case of unusual location on Linux (Flatpack)
endif()

if (BUILD_TESTING)
    find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS Test)
endif()

if (NOT DISABLE_DBUS)
    find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS DBus)
endif()

if (UNIX AND NOT APPLE AND NOT NO_X11)
    add_definitions(-DQZ_WS_X11)
    find_package(XCB REQUIRED COMPONENTS XCB UTIL)
    find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS X11Extras)
endif()

if (WIN32)
        include(${CMAKE_SOURCE_DIR}/vsconfig/msvc.conf)
    find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS WinExtras)
endif()

# Mandatory: OpenSSL

find_package(OpenSSL REQUIRED)

if (NOT WIN32)

    # KF5I18n: Mandatory with downloaded translations (only for ki18n_install)

    if (EXISTS "${CMAKE_SOURCE_DIR}/po")
        find_package(KF5I18n REQUIRED)
    endif()

    # Optional: GnomeKeyring
    find_package(PkgConfig)
    if (PKG_CONFIG_FOUND)
        option(BUILD_KEYRING "Gnome keyring password plugin" ON)
        if (BUILD_KEYRING)
            pkg_check_modules(GNOME_KEYRING IMPORTED_TARGET gnome-keyring-1 )
        endif()
    endif()

    # Optional: KWallet, KIO, KCrash, KCoreAddons
    if (NOT NO_PLUGINS)
        set(KF5_MIN_VERSION "5.54.0")
        find_package(KF5Wallet ${KF5_MIN_VERSION} CONFIG)
        set_package_properties(KF5Wallet PROPERTIES DESCRIPTION "KDE Frameworks Integration plugin" TYPE OPTIONAL)
        find_package(KF5KIO ${KF5_MIN_VERSION} CONFIG)
        set_package_properties(KF5KIO PROPERTIES DESCRIPTION "KDE Frameworks Integration plugin" TYPE OPTIONAL)
        find_package(KF5Crash ${KF5_MIN_VERSION} CONFIG)
        set_package_properties(KF5Crash PROPERTIES DESCRIPTION "KDE Frameworks Integration plugin" TYPE OPTIONAL)
        find_package(KF5CoreAddons ${KF5_MIN_VERSION} CONFIG)
        set_package_properties(KF5CoreAddons PROPERTIES DESCRIPTION "KDE Frameworks Integration plugin" TYPE OPTIONAL)
        find_package(KF5Purpose ${KF5_MIN_VERSION} CONFIG)
        set_package_properties(KF5Purpose PROPERTIES DESCRIPTION "KDE Frameworks Integration plugin" TYPE OPTIONAL)
        find_package(KF5Archive ${KF5_MIN_VERSION} CONFIG)
        set_package_properties(KF5Archive PROPERTIES DESCRIPTION "KDE Frameworks Integration plugin" TYPE OPTIONAL)
        if (KF5Wallet_FOUND AND KF5KIO_FOUND AND KF5Crash_FOUND AND KF5CoreAddons_FOUND AND KF5Purpose_FOUND AND KF5Archive_FOUND)
            set(ENABLE_KDE_FRAMEWORKS_INTEGRATION_PLUGIN TRUE)
        endif()
    endif()
endif()

if (NOT NO_PLUGINS)

    # Optional: PySide2 - This is unlikely to be ever supported in midori

    find_package(PySide2 "2.0.0" CONFIG)
    find_package(Shiboken2 "2.0.0" CONFIG)
    find_package(Python3 COMPONENTS Development)
    set_package_properties(PySide2 PROPERTIES DESCRIPTION "Python plugins" TYPE OPTIONAL)
    set_package_properties(Shiboken2 PROPERTIES DESCRIPTION "Python plugins" TYPE OPTIONAL)
    set_package_properties(Python3 PROPERTIES DESCRIPTION "Python plugins" TYPE OPTIONAL)
    if (PySide2_FOUND AND Shiboken2_FOUND AND Python3_FOUND)
        # clang or gcc dep??? - fatal: 'stddef.h' file not found
        set(ENABLE_PYTHON_PLUGINS TRUE)
    endif()

endif()

find_package(intl)
	if (Intl_FOUND)
    	set(HAVE_LIBINTL TRUE)
endif()

# Git revision

if (EXISTS "${CMAKE_SOURCE_DIR}/.git")
  find_package(Git QUIET)
  if(GIT_FOUND)
    execute_process(
      COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      OUTPUT_VARIABLE GIT_REVISION
    )
    string(REGEX REPLACE "\n" "" GIT_REVISION "${GIT_REVISION}")
    set(MIDORI_GIT_REVISION "${GIT_REVISION}")
  else()
    message(STATUS "Git revision could not be determined")
  endif()
endif()

configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/src/config.h)

# Include dirs used everywhere

include_directories(
     ${CMAKE_SOURCE_DIR}/src/lib/3rdparty
     ${CMAKE_SOURCE_DIR}/src/lib/adblock
     ${CMAKE_SOURCE_DIR}/src/lib/app
     ${CMAKE_SOURCE_DIR}/src/lib/autofill
     ${CMAKE_SOURCE_DIR}/src/lib/bookmarks
     ${CMAKE_SOURCE_DIR}/src/lib/cookies
     ${CMAKE_SOURCE_DIR}/src/lib/downloads
     ${CMAKE_SOURCE_DIR}/src/lib/history
     ${CMAKE_SOURCE_DIR}/src/lib/navigation
     ${CMAKE_SOURCE_DIR}/src/lib/network
     ${CMAKE_SOURCE_DIR}/src/lib/notifications
     ${CMAKE_SOURCE_DIR}/src/lib/opensearch
     ${CMAKE_SOURCE_DIR}/src/lib/other
     ${CMAKE_SOURCE_DIR}/src/lib/popupwindow
     ${CMAKE_SOURCE_DIR}/src/lib/preferences
     ${CMAKE_SOURCE_DIR}/src/lib/session
     ${CMAKE_SOURCE_DIR}/src/lib/sidebar
     ${CMAKE_SOURCE_DIR}/src/lib/tabwidget
     ${CMAKE_SOURCE_DIR}/src/lib/tools
     ${CMAKE_SOURCE_DIR}/src/lib/webengine
     ${CMAKE_SOURCE_DIR}/src/lib/webtab
)

if (NOT NO_PLUGINS)
include_directories(
     ${CMAKE_SOURCE_DIR}/src/lib/plugins
)
endif()

# Finally, go into the subdirs

add_subdirectory(src)

if (BUILD_TESTING)
    add_subdirectory(autotests)
    add_subdirectory(tests/benchmarks)
endif()

# Tell releaseme that po is already taken care of
# SKIP_PO_INSTALL

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
