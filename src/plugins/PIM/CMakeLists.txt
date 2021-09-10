set( PIM_SRCS
	PIM_plugin.cpp
	PIM_handler.cpp
	PIM_settings.cpp
	)

ecm_create_qm_loader( PIM_SRCS bhawk_pim_qt )

set( PIM_UIS
	PIM_settings.ui
	)
qt5_wrap_ui(UIS ${PIM_UIS})

set( PIM_RSCS
	PIM_res.qrc
	)
qt5_add_resources(RSCS ${PIM_RSCS})

add_library(PIM MODULE ${PIM_SRCS} ${UIS} ${RSCS})
install(TARGETS PIM DESTINATION ${MIDORI_INSTALL_PLUGINDIR})
target_link_libraries(PIM MidoriPrivate)

if (NOT DEBUG)
    target_precompile_headers(PIM PRIVATE
        ${CMAKE_SOURCE_DIR}/src/plugins/PIM/PIM_handler.h
        ${CMAKE_SOURCE_DIR}/src/plugins/PIM/PIM_plugin.h
        ${CMAKE_SOURCE_DIR}/src/plugins/PIM/PIM_settings.h
    )
endif()
