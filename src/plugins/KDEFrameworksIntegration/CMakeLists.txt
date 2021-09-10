set(KDEFrameworksIntegration_SRCS
    kdeframeworksintegrationplugin.cpp
    kwalletpasswordbackend.cpp
    kioschemehandler.cpp
)

ecm_create_qm_loader(KDEFrameworksIntegration_SRCS bhawk_kdeframeworksintegration_qt)

set(KDEFrameworksIntegration_RSCS
    kdeframeworksintegration.qrc
)
qt5_add_resources(RSCS ${KDEFrameworksIntegration_RSCS})

add_library(KDEFrameworksIntegration MODULE ${KDEFrameworksIntegration_SRCS} ${RSCS})
install(TARGETS KDEFrameworksIntegration DESTINATION ${BHAWK_INSTALL_PLUGINDIR})
target_link_libraries(KDEFrameworksIntegration
    BhawkPrivate
    KF5::Wallet
    KF5::KIOCore
    KF5::KIOWidgets
    KF5::Crash
    KF5::CoreAddons
    KF5::PurposeWidgets
)

if (NOT DEBUG)
    target_precompile_headers(KDEFrameworksIntegration PRIVATE
        ${CMAKE_SOURCE_DIR}/src/plugins/KDEFrameworksIntegration/kdeframeworksintegrationplugin.h
        ${CMAKE_SOURCE_DIR}/src/plugins/KDEFrameworksIntegration/kioschemehandler.h
        ${CMAKE_SOURCE_DIR}/src/plugins/KDEFrameworksIntegration/kwalletpasswordbackend.h
    )
endif()
