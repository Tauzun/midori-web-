set( TabManager_SRCS
	tabmanagerplugin.cpp
	tabmanagerwidget.cpp
	tabmanagerwidgetcontroller.cpp
	tabmanagersettings.cpp
	tabmanagerdelegate.cpp
	tldextractor/tldextractor.cpp
	)

ecm_create_qm_loader( TabManager_SRCS bhawk_tabmanager_qt )

set( TabManager_UIS
	tabmanagerwidget.ui
	tabmanagersettings.ui
	)
qt5_wrap_ui(UIS ${TabManager_UIS})

set( TabManager_RSCS
	tabmanagerplugin.qrc
	tldextractor/tldextractor.qrc
	)
qt5_add_resources(RSCS ${TabManager_RSCS})

add_library(TabManager MODULE ${TabManager_SRCS} ${UIS} ${RSCS})
install(TARGETS TabManager DESTINATION ${MIDORI_INSTALL_PLUGINDIR})
target_link_libraries(TabManager MidoriPrivate)
if (NOT DEBUG)
    target_precompile_headers(TabManager PRIVATE
        ${CMAKE_SOURCE_DIR}/src/plugins/TabManager/tldextractor/tldextractor.h
        ${CMAKE_SOURCE_DIR}/src/plugins/TabManager/tabmanagerdelegate.h
        ${CMAKE_SOURCE_DIR}/src/plugins/TabManager/tabmanagerplugin.h
        ${CMAKE_SOURCE_DIR}/src/plugins/TabManager/tabmanagersettings.h
        ${CMAKE_SOURCE_DIR}/src/plugins/TabManager/tabmanagerwidget.h
        ${CMAKE_SOURCE_DIR}/src/plugins/TabManager/tabmanagerwidgetcontroller.h
    )
endif()
