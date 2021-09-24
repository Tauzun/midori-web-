include_directories(
  ../lib/3rdparty
  ../lib/app
  ../lib/session
  ../lib/webengine
  ../lib/webtab
)
set(SRCS main.cpp)
if (WIN32)
    set(SRCS ${SRCS} version.rc)
    configure_file(midoriversion.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/midoriversion.h)
endif()
ecm_create_qm_loader(SRCS bhawk_qt)
add_executable(midori ${SRCS})

target_link_libraries(midori MidoriPrivate)

if (APPLE)
    set_target_properties(midori PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist)
    set_target_properties(midori PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.browser.bhawk")
    set_target_properties(midori PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Midori")
    install(FILES lib/data/icons/exeicons/midori.icns DESTINATION ${BUNDLE_INSTALL_DIR}/midori.app/Contents/Resources)
    # APPLE locale - Apple builds are untested and likely don't work. hashed out line locale below might depend upon Intl?
    #install(DIRECTORY ${CMAKE_BINARY_DIR}/bin/locale DESTINATION ${BUNDLE_INSTALL_DIR}/midori.app/Contents/Resources)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/themes DESTINATION ${BUNDLE_INSTALL_DIR}/midori.app/Contents/Resources)
    install(DIRECTORY ${CMAKE_BINARY_DIR}/bin/plugins DESTINATION ${BUNDLE_INSTALL_DIR}/bhawk.app/Contents/Resources)
endif ()

if (CMAKE_SYSTEM_NAME MATCHES FreeBSD OR CMAKE_SYSTEM_NAME MATCHES OpenBSD)
    target_link_libraries(midori execinfo)
endif()

if (UNIX AND NOT APPLE)
    install(TARGETS midori ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/themes DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/midori)
    if (KF5I18n_FOUND)
        ki18n_install(${CMAKE_SOURCE_DIR}/po)
    endif()
    ecm_install_po_files_as_qm(${CMAKE_SOURCE_DIR}/poqm)

    install(PROGRAMS ../../linux/applications/org.astian.midori.desktop DESTINATION ${KDE_INSTALL_APPDIR})

    install(FILES ../../linux/pixmaps/midori.png DESTINATION ${KDE_INSTALL_DATADIR}/pixmaps)

    ecm_install_icons(ICONS
        ../../linux/hicolor/16-apps-midori.png
        ../../linux/hicolor/32-apps-midori.png
        ../../linux/hicolor/48-apps-midori.png
        ../../linux/hicolor/64-apps-midori.png
        ../../linux/hicolor/128-apps-midori.png
        ../../linux/hicolor/256-apps-midori.png
        ../../linux/hicolor/sc-apps-midori.svg
        DESTINATION ${KDE_INSTALL_ICONDIR} THEME hicolor)

    install(FILES ../../linux/appdata/org.astian.midori.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
endif()

