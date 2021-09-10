set( TestPlugin_SRCS
	testplugin.cpp
	testplugin_sidebar.cpp
	)

ecm_create_qm_loader( TestPlugin_SRCS bhawk_testplugin_qt )

add_library(TestPlugin MODULE ${TestPlugin_SRCS} ${RSCS})
install(TARGETS TestPlugin DESTINATION ${MIDORI_INSTALL_PLUGINDIR})
target_link_libraries(TestPlugin MidoriPrivate)
if (NOT DEBUG)
    target_precompile_headers(TestPlugin PRIVATE
        ${CMAKE_SOURCE_DIR}/src/plugins/TestPlugin/testplugin.h
        ${CMAKE_SOURCE_DIR}/src/plugins/TestPlugin/testplugin_sidebar.h
    )
endif()