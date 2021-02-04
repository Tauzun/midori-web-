set( DisableJS_SRCS
	disablejs.cpp
	disablejs_icon.cpp
	disablejs_manager.cpp
	)

ecm_create_qm_loader( DisableJS_SRCS bhawk_disablejs_qt )

set( DisableJS_RSCS
	disablejs.qrc
	)
qt5_add_resources(RSCS ${DisableJS_RSCS})

add_library(DisableJS MODULE ${DisableJS_SRCS} ${RSCS})
install(TARGETS DisableJS DESTINATION ${MIDORI_INSTALL_PLUGINDIR})
target_link_libraries(DisableJS MidoriPrivate)

if (NOT DEBUG)
    target_precompile_headers(DisableJS PRIVATE
        ${CMAKE_SOURCE_DIR}/src/plugins/DisableJS/disablejs.h
        ${CMAKE_SOURCE_DIR}/src/plugins/DisableJS/disablejs_icon.h
        ${CMAKE_SOURCE_DIR}/src/plugins/DisableJS/disablejs_manager.h
    )
endif()
