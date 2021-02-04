include_directories(3rdparty)

set( MouseGestures_HDRS
	3rdparty/QjtMouseGestureFilter.h
	)

set( MouseGestures_SRCS
	3rdparty/mousegesturerecognizer.cpp
	3rdparty/QjtMouseGesture.cpp
	3rdparty/QjtMouseGestureFilter.cpp
	3rdparty/adv_recognizer.cpp
	mousegestures.cpp
	mousegesturesplugin.cpp
	mousegesturessettingsdialog.cpp
	)

ecm_create_qm_loader( MouseGestures_SRCS bhawk_mousegestures_qt )

set( MouseGestures_UIS
	mousegesturessettingsdialog.ui
	)
qt5_wrap_ui(UIS ${MouseGestures_UIS})

set( MouseGestures_RSCS
	mousegestures.qrc
	)
qt5_add_resources(RSCS ${MouseGestures_RSCS})

KDE_ENABLE_EXCEPTIONS()

add_library(MouseGestures MODULE ${MouseGestures_SRCS} ${UIS} ${RSCS})
install(TARGETS MouseGestures DESTINATION ${MIDORI_INSTALL_PLUGINDIR})
target_link_libraries(MouseGestures MidoriPrivate)

if (NOT DEBUG)
    target_precompile_headers(MouseGestures PRIVATE
        ${CMAKE_SOURCE_DIR}/src/plugins/MouseGestures/3rdparty/adv_recognizer.h
        ${CMAKE_SOURCE_DIR}/src/plugins/MouseGestures/3rdparty/mousegesturerecognizer.h
        ${CMAKE_SOURCE_DIR}/src/plugins/MouseGestures/3rdparty/QjtMouseGesture.h
        ${CMAKE_SOURCE_DIR}/src/plugins/MouseGestures/3rdparty/QjtMouseGestureFilter.h
        ${CMAKE_SOURCE_DIR}/src/plugins/MouseGestures/3rdparty/ring_buffer.h
        ${CMAKE_SOURCE_DIR}/src/plugins/MouseGestures/mousegestures.h
        ${CMAKE_SOURCE_DIR}/src/plugins/MouseGestures/mousegesturesplugin.h
        ${CMAKE_SOURCE_DIR}/src/plugins/MouseGestures/mousegesturessettingsdialog.h
    )
endif()
