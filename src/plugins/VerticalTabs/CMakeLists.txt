set( VerticalTabs_SRCS
    verticaltabsplugin.cpp
    verticaltabscontroller.cpp
    verticaltabswidget.cpp
    verticaltabssettings.cpp
    tabtreeview.cpp
    tabtreedelegate.cpp
    loadinganimator.cpp
    tabfiltermodel.cpp
    tablistview.cpp
    tablistdelegate.cpp
    verticaltabsschemehandler.cpp
)

ecm_create_qm_loader( VerticalTabs_SRCS bhawk_verticaltabs_qt )

set( VerticalTabs_UIS
    verticaltabssettings.ui
)
qt5_wrap_ui(UIS ${VerticalTabs_UIS})

set( VerticalTabs_RSCS
    verticaltabs.qrc
)
qt5_add_resources(RSCS ${VerticalTabs_RSCS})

add_library(VerticalTabs MODULE ${VerticalTabs_SRCS} ${UIS} ${RSCS})
install(TARGETS VerticalTabs DESTINATION ${MIDORI_INSTALL_PLUGINDIR})
target_link_libraries(VerticalTabs MidoriPrivate)

if (NOT DEBUG)
    target_precompile_headers(VerticalTabs PRIVATE
        ${CMAKE_SOURCE_DIR}/src/plugins/VerticalTabs/loadinganimator.h
        ${CMAKE_SOURCE_DIR}/src/plugins/VerticalTabs/tabfiltermodel.h
        ${CMAKE_SOURCE_DIR}/src/plugins/VerticalTabs/tablistdelegate.h
        ${CMAKE_SOURCE_DIR}/src/plugins/VerticalTabs/tablistview.h
        ${CMAKE_SOURCE_DIR}/src/plugins/VerticalTabs/tabtreedelegate.h
        ${CMAKE_SOURCE_DIR}/src/plugins/VerticalTabs/tabtreeview.h
        ${CMAKE_SOURCE_DIR}/src/plugins/VerticalTabs/verticaltabscontroller.h
        ${CMAKE_SOURCE_DIR}/src/plugins/VerticalTabs/verticaltabsplugin.h
        ${CMAKE_SOURCE_DIR}/src/plugins/VerticalTabs/verticaltabsschemehandler.h
        ${CMAKE_SOURCE_DIR}/src/plugins/VerticalTabs/verticaltabssettings.h
        ${CMAKE_SOURCE_DIR}/src/plugins/VerticalTabs/verticaltabswidget.h
    )
endif()
