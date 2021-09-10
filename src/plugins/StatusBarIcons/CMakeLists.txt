set( StatusBarIcons_SRCS
	statusbariconsplugin.cpp
	sbi_iconsmanager.cpp
	sbi_imagesicon.cpp
	sbi_javascripticon.cpp
	sbi_networkicon.cpp
	sbi_networkproxy.cpp
	sbi_proxywidget.cpp
	sbi_networkicondialog.cpp
	sbi_networkmanager.cpp
	sbi_settingsdialog.cpp
	sbi_icon.cpp
	sbi_zoomwidget.cpp
	)

ecm_create_qm_loader( StatusBarIcons_SRCS bhawk_statusbaricons_qt )

set( StatusBarIcons_UIS
	sbi_proxywidget.ui
	sbi_networkicondialog.ui
	sbi_settingsdialog.ui
	)
qt5_wrap_ui(UIS ${StatusBarIcons_UIS})

set( StatusBarIcons_RSCS
	statusbaricons.qrc
	)
qt5_add_resources(RSCS ${StatusBarIcons_RSCS})

add_library(StatusBarIcons MODULE ${StatusBarIcons_SRCS} ${UIS} ${RSCS})
install(TARGETS StatusBarIcons DESTINATION ${MIDORI_INSTALL_PLUGINDIR})
target_link_libraries(StatusBarIcons MidoriPrivate)
if (NOT DEBUG)
    target_precompile_headers(StatusBarIcons PRIVATE
        ${CMAKE_SOURCE_DIR}/src/plugins/StatusBarIcons/sbi_icon.h
        ${CMAKE_SOURCE_DIR}/src/plugins/StatusBarIcons/sbi_iconsmanager.h
        ${CMAKE_SOURCE_DIR}/src/plugins/StatusBarIcons/sbi_imagesicon.h
        ${CMAKE_SOURCE_DIR}/src/plugins/StatusBarIcons/sbi_javascripticon.h
        ${CMAKE_SOURCE_DIR}/src/plugins/StatusBarIcons/sbi_networkicon.h
        ${CMAKE_SOURCE_DIR}/src/plugins/StatusBarIcons/sbi_networkicondialog.h
        ${CMAKE_SOURCE_DIR}/src/plugins/StatusBarIcons/sbi_networkmanager.h
        ${CMAKE_SOURCE_DIR}/src/plugins/StatusBarIcons/sbi_networkproxy.h
        ${CMAKE_SOURCE_DIR}/src/plugins/StatusBarIcons/sbi_proxywidget.h
        ${CMAKE_SOURCE_DIR}/src/plugins/StatusBarIcons/sbi_settingsdialog.h
        ${CMAKE_SOURCE_DIR}/src/plugins/StatusBarIcons/sbi_zoomwidget.h
        ${CMAKE_SOURCE_DIR}/src/plugins/StatusBarIcons/statusbariconsplugin.h
    )
endif()
