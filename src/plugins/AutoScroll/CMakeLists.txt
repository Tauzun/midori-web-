set( AutoScroll_SRCS
	autoscrollplugin.cpp
	autoscroller.cpp
	framescroller.cpp
	autoscrollsettings.cpp
	)

ecm_create_qm_loader( AutoScroll_SRCS bhawk_autoscroll_qt )

set( AutoScroll_UIS
	autoscrollsettings.ui
	)
qt5_wrap_ui(UIS ${AutoScroll_UIS})

set( AutoScroll_RSCS
	autoscroll.qrc
	)
qt5_add_resources(RSCS ${AutoScroll_RSCS})

add_library(AutoScroll MODULE ${AutoScroll_SRCS} ${UIS} ${RSCS})
install(TARGETS AutoScroll DESTINATION ${MIDORI_INSTALL_PLUGINDIR})
target_link_libraries(AutoScroll MidoriPrivate)

if (NOT DEBUG)
    target_precompile_headers(AutoScroll PRIVATE
        ${CMAKE_SOURCE_DIR}/src/plugins/AutoScroll/autoscroller.h
        ${CMAKE_SOURCE_DIR}/src/plugins/AutoScroll/autoscrollplugin.h
        ${CMAKE_SOURCE_DIR}/src/plugins/AutoScroll/autoscrollsettings.h
        ${CMAKE_SOURCE_DIR}/src/plugins/AutoScroll/framescroller.h
    )
endif()
