set( FlashCookieManager_SRCS
	fcm_plugin.cpp
	fcm_dialog.cpp
	fcm_notification.cpp
	)

ecm_create_qm_loader( FlashCookieManager_SRCS bhawk_flashcookiemanager_qt )

set( FlashCookieManager_UIS
	fcm_dialog.ui
	fcm_notification.ui
	)
qt5_wrap_ui(UIS ${FlashCookieManager_UIS})

set( FlashCookieManager_RSCS
	flashcookiemanager.qrc
	)
qt5_add_resources(RSCS ${FlashCookieManager_RSCS})

add_library(FlashCookieManager MODULE ${FlashCookieManager_SRCS} ${UIS} ${RSCS})
install(TARGETS FlashCookieManager DESTINATION ${MIDORI_INSTALL_PLUGINDIR})
target_link_libraries(FlashCookieManager MidoriPrivate)

if (NOT DEBUG)
    target_precompile_headers(FlashCookieManager PRIVATE
        ${CMAKE_SOURCE_DIR}/src/plugins/FlashCookieManager/fcm_dialog.h
        ${CMAKE_SOURCE_DIR}/src/plugins/FlashCookieManager/fcm_notification.h
        ${CMAKE_SOURCE_DIR}/src/plugins/FlashCookieManager/fcm_plugin.h
    )
endif()
