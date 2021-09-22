add_definitions(-DBHAWK_SHAREDLIBRARY)

set(SRCS
    3rdparty/qtsingleapplication/qtsingleapplication.cpp
    3rdparty/qtsingleapplication/qtlocalpeer.cpp
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(SRCS ${SRCS} ${CMAKE_SOURCE_DIR}/tests/modeltest/modeltest.cpp)
    include_directories(${CMAKE_SOURCE_DIR}/tests/modeltest)
endif()

include_directories(
    3rdparty
    app
    autofill
    bookmarks
    cookies
    downloads
    history
    navigation
    network
    notifications
    opensearch
    other
    plugins
    popupwindow
    preferences
    session
    sidebar
    tabwidget
    tools
    webengine
    webtab
)

if (NOT NO_PLUGINS)

include_directories(
    adblock
)

endif()

include_directories(${OPENSSL_INCLUDE_DIR})

set(SRCS ${SRCS}
    3rdparty/fancytabwidget.cpp
    3rdparty/lineedit.cpp
    3rdparty/processinfo.cpp
    3rdparty/squeezelabelv1.cpp
    3rdparty/squeezelabelv2.cpp
    3rdparty/stylehelper.cpp
    adblock/adblockaddsubscriptiondialog.cpp
    adblock/adblockurlinterceptor.cpp
    adblock/adblockdialog.cpp
    adblock/adblockicon.cpp
    adblock/adblockmanager.cpp
    adblock/adblockmatcher.cpp
    adblock/adblockrule.cpp
    adblock/adblocksearchtree.cpp
    adblock/adblocksubscription.cpp
    adblock/adblocktreewidget.cpp
    adblock/adblockplugin.cpp
    app/autosaver.cpp
    app/browserwindow.cpp
    app/commandlineoptions.cpp
    app/datapaths.cpp
    app/mainapplication.cpp
    app/mainmenu.cpp
    app/profilemanager.cpp
    app/proxystyle.cpp
    app/qzcommon.cpp
    app/settings.cpp
    autofill/autofill.cpp
    autofill/autofillicon.cpp
    autofill/autofillnotification.cpp
    autofill/autofillwidget.cpp
    autofill/passwordbackends/databaseencryptedpasswordbackend.cpp
    autofill/passwordbackends/databasepasswordbackend.cpp
    autofill/passwordbackends/passwordbackend.cpp
    autofill/passwordmanager.cpp
    bookmarks/bookmarkitem.cpp
    bookmarks/bookmarks.cpp
    bookmarks/bookmarksexport/bookmarksexportdialog.cpp
    bookmarks/bookmarksexport/bookmarksexporter.cpp
    bookmarks/bookmarksexport/htmlexporter.cpp
    bookmarks/bookmarksicon.cpp
    bookmarks/bookmarksimport/bookmarksimportdialog.cpp
    bookmarks/bookmarksimport/bookmarksimporter.cpp
    bookmarks/bookmarksimport/firefoximporter.cpp
    bookmarks/bookmarksimport/htmlimporter.cpp
    bookmarks/bookmarksimport/chromeimporter.cpp
    bookmarks/bookmarksimport/ieimporter.cpp
    bookmarks/bookmarksimport/operaimporter.cpp
    bookmarks/bookmarksitemdelegate.cpp
    bookmarks/bookmarksmanager.cpp
    bookmarks/bookmarksmenu.cpp
    bookmarks/bookmarksmodel.cpp
    bookmarks/bookmarkstoolbarbutton.cpp
    bookmarks/bookmarkstoolbar.cpp
    bookmarks/bookmarkstools.cpp
    bookmarks/bookmarkstreeview.cpp
    bookmarks/bookmarkswidget.cpp
    cookies/cookiejar.cpp
    cookies/cookiemanager.cpp
    downloads/downloaditem.cpp
    downloads/downloadmanager.cpp
    downloads/downloadoptionsdialog.cpp
    downloads/downloadsbutton.cpp
    history/history.cpp
    history/historyitem.cpp
    history/historymanager.cpp
    history/historymenu.cpp
    history/historymodel.cpp
    history/historytreeview.cpp
    navigation/completer/locationcompleter.cpp
    navigation/completer/locationcompleterdelegate.cpp
    navigation/completer/locationcompletermodel.cpp
    navigation/completer/locationcompleterrefreshjob.cpp
    navigation/completer/locationcompleterview.cpp
    navigation/downicon.cpp
    navigation/goicon.cpp
    navigation/locationbar.cpp
    navigation/locationbarpopup.cpp
    navigation/navigationbar.cpp
    navigation/navigationbartoolbutton.cpp
    navigation/navigationbarconfigdialog.cpp
    navigation/navigationcontainer.cpp
    navigation/reloadstopbutton.cpp
    navigation/siteicon.cpp
    navigation/websearchbar.cpp
    network/networkmanager.cpp
    network/networkurlinterceptor.cpp
    network/schemehandlers/extensionschemehandler.cpp
    network/schemehandlers/bhawkschemehandler.cpp
    network/sslerrordialog.cpp
    notifications/desktopnotification.cpp
    notifications/desktopnotificationsfactory.cpp
    opensearch/editsearchengine.cpp
    opensearch/opensearchengine.cpp
    opensearch/opensearchenginedelegate.cpp
    opensearch/opensearchreader.cpp
    opensearch/searchenginesdialog.cpp
    opensearch/searchenginesmanager.cpp
    other/aboutdialog.cpp
    other/browsinglibrary.cpp
    other/clearprivatedata.cpp
    other/checkboxdialog.cpp
    other/iconchooser.cpp
    other/licenseviewer.cpp
    other/qzsettings.cpp
    other/siteinfo.cpp
    other/siteinfowidget.cpp
    other/statusbar.cpp
    other/updater.cpp
    other/useragentmanager.cpp
    other/protocolhandlerdialog.cpp
    other/protocolhandlermanager.cpp
    plugins/pluginproxy.cpp
    plugins/plugins.cpp
    plugins/speeddial.cpp
    plugins/qml/qmlpluginloader.cpp
    plugins/qml/qmlplugin.cpp
    plugins/qml/qmlplugins.cpp
    plugins/qml/qmlplugininterface.cpp
    plugins/qml/qmlengine.cpp
    plugins/qml/qmlstaticdata.cpp
    plugins/qml/api/bookmarks/qmlbookmarktreenode.cpp
    plugins/qml/api/bookmarks/qmlbookmarks.cpp
    plugins/qml/api/topsites/qmlmostvisitedurl.cpp
    plugins/qml/api/topsites/qmltopsites.cpp
    plugins/qml/api/history/qmlhistoryitem.cpp
    plugins/qml/api/history/qmlhistory.cpp
    plugins/qml/api/cookies/qmlcookie.cpp
    plugins/qml/api/cookies/qmlcookies.cpp
    plugins/qml/api/tabs/qmltab.cpp
    plugins/qml/api/tabs/qmltabs.cpp
    plugins/qml/api/notifications/qmlnotifications.cpp
    plugins/qml/api/clipboard/qmlclipboard.cpp
    plugins/qml/api/windows/qmlwindow.cpp
    plugins/qml/api/windows/qmlwindows.cpp
    plugins/qml/api/browseraction/qmlbrowseraction.cpp
    plugins/qml/api/sidebar/qmlsidebar.cpp
    plugins/qml/api/menus/qmlmenu.cpp
    plugins/qml/api/menus/qmlaction.cpp
    plugins/qml/api/menus/qmlwebhittestresult.cpp
    plugins/qml/api/settings/qmlsettings.cpp
    plugins/qml/api/events/qmlqzobjects.cpp
    plugins/qml/api/events/qmlmouseevent.cpp
    plugins/qml/api/events/qmlwheelevent.cpp
    plugins/qml/api/events/qmlkeyevent.cpp
    plugins/qml/api/userscript/qmluserscript.cpp
    plugins/qml/api/userscript/qmluserscripts.cpp
    plugins/qml/api/userscript/qmlexternaljsobject.cpp
    plugins/qml/api/extensionscheme/qmlextensionscheme.cpp
    plugins/qml/api/extensionscheme/qmlwebengineurlrequestjob.cpp
    plugins/qml/api/fileutils/qmlfileutils.cpp
    plugins/qml/api/qmlenums.cpp
    popupwindow/popuplocationbar.cpp
    popupwindow/popupstatusbarmessage.cpp
    popupwindow/popupwebview.cpp
    popupwindow/popupwindow.cpp
    preferences/acceptlanguage.cpp
    preferences/autofillmanager.cpp
    preferences/jsoptions.cpp
    preferences/pluginlistdelegate.cpp
    preferences/pluginsmanager.cpp
    preferences/preferences.cpp
    preferences/thememanager.cpp
    preferences/useragentdialog.cpp
    session/recoveryjsobject.cpp
    session/restoremanager.cpp
    session/sessionmanager.cpp
    session/sessionmanagerdialog.cpp
    sidebar/bookmarkssidebar.cpp
    sidebar/historysidebar.cpp
    sidebar/sidebar.cpp
    tabwidget/combotabbar.cpp
    tabwidget/tabbar.cpp
    tabwidget/tabicon.cpp
    tabwidget/tabmodel.cpp
    tabwidget/tabmrumodel.cpp
    tabwidget/tabtreemodel.cpp
    tabwidget/tabstackedwidget.cpp
    tabwidget/tabwidget.cpp
    tabwidget/tabcontextmenu.cpp
    tools/abstractbuttoninterface.cpp
    tools/aesinterface.cpp
    tools/animatedwidget.cpp
    tools/buttonwithmenu.cpp
    tools/certificateinfowidget.cpp
    tools/clickablelabel.cpp
    tools/closedtabsmanager.cpp
    tools/closedwindowsmanager.cpp
    tools/colors.cpp
    tools/delayedfilewatcher.cpp
    tools/desktopfile.cpp
    tools/docktitlebarwidget.cpp
    tools/enhancedmenu.cpp
    tools/focusselectlineedit.cpp
    tools/headerview.cpp
    tools/horizontallistwidget.cpp
    tools/html5permissions/html5permissionsdialog.cpp
    tools/html5permissions/html5permissionsmanager.cpp
    tools/html5permissions/html5permissionsnotification.cpp
    tools/iconprovider.cpp
    tools/listitemdelegate.cpp
    tools/mactoolbutton.cpp
    tools/menubar.cpp
    tools/pagethumbnailer.cpp
    tools/progressbar.cpp
    tools/qztools.cpp
    tools/removeitemfocusdelegate.cpp
    tools/scripts.cpp
    tools/sqldatabase.cpp
    tools/toolbutton.cpp
    tools/treewidget.cpp
    tools/wheelhelper.cpp
    webengine/javascript/autofilljsobject.cpp
    webengine/javascript/externaljsobject.cpp
    webengine/loadrequest.cpp
    webengine/webhittestresult.cpp
    webengine/webinspector.cpp
    webengine/webpage.cpp
    webengine/webview.cpp
    webengine/webscrollbar.cpp
    webengine/webscrollbarmanager.cpp
    webtab/searchtoolbar.cpp
    webtab/tabbedwebview.cpp
    webtab/webtab.cpp
)


if (ENABLE_KDE_FRAMEWORKS_INTEGRATION_PLUGIN)
    set(SRCS ${SRCS}
        plugins/ocssupport.cpp
    )
endif()
# Add the logging category for Midori
ecm_qt_declare_logging_category(LOGGING_SRCS HEADER Midori_private_debug.h IDENTIFIER "MIDORI_PRIVATE_LOG" CATEGORY_NAME "org.astian.midori")

if (HAVE_LIBINTL)
        set(SRCS ${SRCS} plugins/qml/api/i18n/qmli18n.cpp)
endif()

if(WIN32)
    set(SRCS ${SRCS} other/registerqappassociation.cpp)
endif()

if (APPLE)
    set(SRCS ${SRCS} tools/disablewindowtabbbing.mm)
endif()
# TODO: use ki18n_wrap_ui?
qt5_wrap_ui(SRCS
    adblock/adblockaddsubscriptiondialog.ui
    adblock/adblockdialog.ui
    autofill/autofillnotification.ui
    autofill/autofillwidget.ui
    autofill/passwordbackends/masterpassworddialog.ui
    bookmarks/bookmarksexport/bookmarksexportdialog.ui
    bookmarks/bookmarksimport/bookmarksimportdialog.ui
    bookmarks/bookmarksmanager.ui
    bookmarks/bookmarkswidget.ui
    cookies/cookiemanager.ui
    downloads/downloaditem.ui
    downloads/downloadmanager.ui
    downloads/downloadoptionsdialog.ui
    history/historymanager.ui
    navigation/navigationbarconfigdialog.ui
    network/sslerrordialog.ui
    notifications/desktopnotification.ui
    opensearch/editsearchengine.ui
    opensearch/searchenginesdialog.ui
    other/aboutdialog.ui
    other/browsinglibrary.ui
    other/clearprivatedata.ui
    other/iconchooser.ui
    other/protocolhandlerdialog.ui
    other/siteinfo.ui
    other/siteinfowidget.ui
    preferences/acceptlanguage.ui
    preferences/addacceptlanguage.ui
    preferences/autofillmanager.ui
    preferences/jsoptions.ui
    preferences/pluginslist.ui
    preferences/preferences.ui
    preferences/thememanager.ui
    preferences/useragentdialog.ui
    session/sessionmanagerdialog.ui
    sidebar/bookmarkssidebar.ui
    sidebar/historysidebar.ui
    tools/certificateinfowidget.ui
    tools/docktitlebarwidget.ui
    tools/html5permissions/html5permissionsdialog.ui
    tools/html5permissions/html5permissionsnotification.ui
    webengine/jsalert.ui
    webengine/jsconfirm.ui
    webengine/jsprompt.ui
    webtab/searchtoolbar.ui
)

qt5_add_resources(SRCS
    adblock/adblock.qrc
    data/data.qrc
    data/html.qrc
    data/icons.qrc
)

add_library(MidoriPrivate SHARED ${SRCS} ${LOGGING_SRCS})

get_property(QT_WEBENGINE_INCLUDE_DIRS TARGET Qt5::WebEngine PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(MidoriPrivate SYSTEM PUBLIC ${QT_WEBENGINE_INCLUDE_DIRS})

target_link_libraries(MidoriPrivate
    Qt5::Widgets
    Qt5::WebEngineWidgets
    Qt5::Network
    Qt5::Sql
    Qt5::PrintSupport
    Qt5::QuickWidgets
    Qt5::WebChannel
    ${OPENSSL_CRYPTO_LIBRARY}
)

    if (HAVE_LIBINTL)
	if(WIN32)
        	target_link_libraries(MidoriPrivate unofficial::gettext::libintl) ### Your libintl might be different. Here we use vcpkg's version.
	else()
    		target_include_directories(MidoriPrivate SYSTEM PUBLIC ${Intl_INCLUDE_DIRS})
    		target_link_libraries(MidoriPrivate ${Intl_LIBRARIES})

	endif()
    endif()

if (ENABLE_KDE_FRAMEWORKS_INTEGRATION_PLUGIN)
    target_link_libraries(MidoriPrivate KF5::Archive)
endif()

if (UNIX AND NOT APPLE)
    if (NOT NO_X11)
        target_link_libraries(MidoriPrivate XCB::XCB Qt5::X11Extras)
    endif()

    set_target_properties(MidoriPrivate PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION "3")
    install(TARGETS MidoriPrivate ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)
endif()

if (WIN32)
    target_link_libraries(MidoriPrivate Qt5::WinExtras)
endif()

if (APPLE)
    target_link_libraries(MidoriPrivate "-framework CoreServices -framework AppKit")
endif()

if (NOT DISABLE_DBUS)
    target_link_libraries(MidoriPrivate Qt5::DBus)
endif()

if (NOT DEBUG)
    target_precompile_headers(MidoriPrivate PRIVATE
        ${CMAKE_SOURCE_DIR}/src/lib/3rdparty/qtsingleapplication/qtlocalpeer.h
        ${CMAKE_SOURCE_DIR}/src/lib/3rdparty/qtsingleapplication/qtlockedfile.h
        ${CMAKE_SOURCE_DIR}/src/lib/3rdparty/qtsingleapplication/qtsingleapplication.h
        ${CMAKE_SOURCE_DIR}/src/lib/3rdparty/fancytabwidget.h
        ${CMAKE_SOURCE_DIR}/src/lib/3rdparty/lineedit.h
        ${CMAKE_SOURCE_DIR}/src/lib/3rdparty/processinfo.h
        ${CMAKE_SOURCE_DIR}/src/lib/3rdparty/squeezelabelv1.h
        ${CMAKE_SOURCE_DIR}/src/lib/3rdparty/squeezelabelv2.h
        ${CMAKE_SOURCE_DIR}/src/lib/3rdparty/stylehelper.h
        ${CMAKE_SOURCE_DIR}/src/lib/adblock/adblockaddsubscriptiondialog.h
        ${CMAKE_SOURCE_DIR}/src/lib/adblock/adblockdialog.h
        ${CMAKE_SOURCE_DIR}/src/lib/adblock/adblockicon.h
        ${CMAKE_SOURCE_DIR}/src/lib/adblock/adblockmanager.h
        ${CMAKE_SOURCE_DIR}/src/lib/adblock/adblockmatcher.h
        ${CMAKE_SOURCE_DIR}/src/lib/adblock/adblockplugin.h
        ${CMAKE_SOURCE_DIR}/src/lib/adblock/adblockrule.h
        ${CMAKE_SOURCE_DIR}/src/lib/adblock/adblocksearchtree.h
        ${CMAKE_SOURCE_DIR}/src/lib/adblock/adblocksubscription.h
        ${CMAKE_SOURCE_DIR}/src/lib/adblock/adblocktreewidget.h
        ${CMAKE_SOURCE_DIR}/src/lib/adblock/adblockurlinterceptor.h
        ${CMAKE_SOURCE_DIR}/src/lib/app/autosaver.h
        ${CMAKE_SOURCE_DIR}/src/lib/app/browserwindow.h
        ${CMAKE_SOURCE_DIR}/src/lib/app/commandlineoptions.h
        ${CMAKE_SOURCE_DIR}/src/lib/app/datapaths.h
        ${CMAKE_SOURCE_DIR}/src/lib/app/mainapplication.h
        ${CMAKE_SOURCE_DIR}/src/lib/app/mainmenu.h
        ${CMAKE_SOURCE_DIR}/src/lib/app/profilemanager.h
        ${CMAKE_SOURCE_DIR}/src/lib/app/proxystyle.h
        ${CMAKE_SOURCE_DIR}/src/lib/app/qzcommon.h
        ${CMAKE_SOURCE_DIR}/src/lib/app/settings.h
        ${CMAKE_SOURCE_DIR}/src/lib/autofill/passwordbackends/databaseencryptedpasswordbackend.h
        ${CMAKE_SOURCE_DIR}/src/lib/autofill/passwordbackends/databasepasswordbackend.h
        ${CMAKE_SOURCE_DIR}/src/lib/autofill/passwordbackends/passwordbackend.h
        ${CMAKE_SOURCE_DIR}/src/lib/autofill/autofill.h
        ${CMAKE_SOURCE_DIR}/src/lib/autofill/autofillicon.h
        ${CMAKE_SOURCE_DIR}/src/lib/autofill/autofillnotification.h
        ${CMAKE_SOURCE_DIR}/src/lib/autofill/autofillwidget.h
        ${CMAKE_SOURCE_DIR}/src/lib/autofill/passwordmanager.h
        ${CMAKE_SOURCE_DIR}/src/lib/bookmarks/bookmarksexport/bookmarksexportdialog.h
        ${CMAKE_SOURCE_DIR}/src/lib/bookmarks/bookmarksexport/bookmarksexporter.h
        ${CMAKE_SOURCE_DIR}/src/lib/bookmarks/bookmarksexport/htmlexporter.h
        ${CMAKE_SOURCE_DIR}/src/lib/bookmarks/bookmarksimport/bookmarksimportdialog.h
        ${CMAKE_SOURCE_DIR}/src/lib/bookmarks/bookmarksimport/bookmarksimporter.h
        ${CMAKE_SOURCE_DIR}/src/lib/bookmarks/bookmarksimport/chromeimporter.h
        ${CMAKE_SOURCE_DIR}/src/lib/bookmarks/bookmarksimport/firefoximporter.h
        ${CMAKE_SOURCE_DIR}/src/lib/bookmarks/bookmarksimport/htmlimporter.h
        ${CMAKE_SOURCE_DIR}/src/lib/bookmarks/bookmarksimport/ieimporter.h
        ${CMAKE_SOURCE_DIR}/src/lib/bookmarks/bookmarksimport/operaimporter.h
        ${CMAKE_SOURCE_DIR}/src/lib/bookmarks/bookmarkitem.h
        ${CMAKE_SOURCE_DIR}/src/lib/bookmarks/bookmarks.h
        ${CMAKE_SOURCE_DIR}/src/lib/bookmarks/bookmarksicon.h
        ${CMAKE_SOURCE_DIR}/src/lib/bookmarks/bookmarksitemdelegate.h
        ${CMAKE_SOURCE_DIR}/src/lib/bookmarks/bookmarksmanager.h
        ${CMAKE_SOURCE_DIR}/src/lib/bookmarks/bookmarksmenu.h
        ${CMAKE_SOURCE_DIR}/src/lib/bookmarks/bookmarksmodel.h
        ${CMAKE_SOURCE_DIR}/src/lib/bookmarks/bookmarkstoolbar.h
        ${CMAKE_SOURCE_DIR}/src/lib/bookmarks/bookmarkstoolbarbutton.h
        ${CMAKE_SOURCE_DIR}/src/lib/bookmarks/bookmarkstools.h
        ${CMAKE_SOURCE_DIR}/src/lib/bookmarks/bookmarkstreeview.h
        ${CMAKE_SOURCE_DIR}/src/lib/bookmarks/bookmarkswidget.h
        ${CMAKE_SOURCE_DIR}/src/lib/cookies/cookiejar.h
        ${CMAKE_SOURCE_DIR}/src/lib/cookies/cookiemanager.h
        ${CMAKE_SOURCE_DIR}/src/lib/downloads/downloaditem.h
        ${CMAKE_SOURCE_DIR}/src/lib/downloads/downloadmanager.h
        ${CMAKE_SOURCE_DIR}/src/lib/downloads/downloadoptionsdialog.h
        ${CMAKE_SOURCE_DIR}/src/lib/downloads/downloadsbutton.h
        ${CMAKE_SOURCE_DIR}/src/lib/history/history.h
        ${CMAKE_SOURCE_DIR}/src/lib/history/historyitem.h
        ${CMAKE_SOURCE_DIR}/src/lib/history/historymanager.h
        ${CMAKE_SOURCE_DIR}/src/lib/history/historymenu.h
        ${CMAKE_SOURCE_DIR}/src/lib/history/historymodel.h
        ${CMAKE_SOURCE_DIR}/src/lib/history/historytreeview.h
        ${CMAKE_SOURCE_DIR}/src/lib/navigation/completer/locationcompleter.h
        ${CMAKE_SOURCE_DIR}/src/lib/navigation/completer/locationcompleterdelegate.h
        ${CMAKE_SOURCE_DIR}/src/lib/navigation/completer/locationcompletermodel.h
        ${CMAKE_SOURCE_DIR}/src/lib/navigation/completer/locationcompleterrefreshjob.h
        ${CMAKE_SOURCE_DIR}/src/lib/navigation/completer/locationcompleterview.h
        ${CMAKE_SOURCE_DIR}/src/lib/navigation/downicon.h
        ${CMAKE_SOURCE_DIR}/src/lib/navigation/goicon.h
        ${CMAKE_SOURCE_DIR}/src/lib/navigation/locationbar.h
        ${CMAKE_SOURCE_DIR}/src/lib/navigation/locationbarpopup.h
        ${CMAKE_SOURCE_DIR}/src/lib/navigation/navigationbar.h
        ${CMAKE_SOURCE_DIR}/src/lib/navigation/navigationbarconfigdialog.h
        ${CMAKE_SOURCE_DIR}/src/lib/navigation/navigationbartoolbutton.h
        ${CMAKE_SOURCE_DIR}/src/lib/navigation/navigationcontainer.h
        ${CMAKE_SOURCE_DIR}/src/lib/navigation/reloadstopbutton.h
        ${CMAKE_SOURCE_DIR}/src/lib/navigation/siteicon.h
        ${CMAKE_SOURCE_DIR}/src/lib/navigation/websearchbar.h
        ${CMAKE_SOURCE_DIR}/src/lib/network/schemehandlers/extensionschemehandler.h
        ${CMAKE_SOURCE_DIR}/src/lib/network/schemehandlers/bhawkschemehandler.h
        ${CMAKE_SOURCE_DIR}/src/lib/network/networkmanager.h
        ${CMAKE_SOURCE_DIR}/src/lib/network/networkurlinterceptor.h
        ${CMAKE_SOURCE_DIR}/src/lib/network/sslerrordialog.h
        ${CMAKE_SOURCE_DIR}/src/lib/network/urlinterceptor.h
        ${CMAKE_SOURCE_DIR}/src/lib/notifications/desktopnotification.h
        ${CMAKE_SOURCE_DIR}/src/lib/notifications/desktopnotificationsfactory.h
        ${CMAKE_SOURCE_DIR}/src/lib/opensearch/editsearchengine.h
        ${CMAKE_SOURCE_DIR}/src/lib/opensearch/opensearchengine.h
        ${CMAKE_SOURCE_DIR}/src/lib/opensearch/opensearchenginedelegate.h
        ${CMAKE_SOURCE_DIR}/src/lib/opensearch/opensearchreader.h
        ${CMAKE_SOURCE_DIR}/src/lib/opensearch/searchenginesdialog.h
        ${CMAKE_SOURCE_DIR}/src/lib/opensearch/searchenginesmanager.h
        ${CMAKE_SOURCE_DIR}/src/lib/other/aboutdialog.h
        ${CMAKE_SOURCE_DIR}/src/lib/other/browsinglibrary.h
        ${CMAKE_SOURCE_DIR}/src/lib/other/checkboxdialog.h
        ${CMAKE_SOURCE_DIR}/src/lib/other/clearprivatedata.h
        ${CMAKE_SOURCE_DIR}/src/lib/other/iconchooser.h
        ${CMAKE_SOURCE_DIR}/src/lib/other/licenseviewer.h
        ${CMAKE_SOURCE_DIR}/src/lib/other/protocolhandlerdialog.h
        ${CMAKE_SOURCE_DIR}/src/lib/other/protocolhandlermanager.h
        ${CMAKE_SOURCE_DIR}/src/lib/other/qzsettings.h
        ${CMAKE_SOURCE_DIR}/src/lib/other/registerqappassociation.h
        ${CMAKE_SOURCE_DIR}/src/lib/other/siteinfo.h
        ${CMAKE_SOURCE_DIR}/src/lib/other/siteinfowidget.h
        ${CMAKE_SOURCE_DIR}/src/lib/other/statusbar.h
        ${CMAKE_SOURCE_DIR}/src/lib/other/updater.h
        ${CMAKE_SOURCE_DIR}/src/lib/other/useragentmanager.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/pluginproxy.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/ocssupport.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/plugininterface.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/plugins.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/speeddial.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/qmlengine.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/qmlplugin.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/qmlplugininterface.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/qmlpluginloader.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/qmlplugins.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/qmlstaticdata.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/qmlenums.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/bookmarks/qmlbookmarks.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/bookmarks/qmlbookmarktreenode.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/browseraction/qmlbrowseraction.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/clipboard/qmlclipboard.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/cookies/qmlcookie.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/cookies/qmlcookies.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/events/qmlkeyevent.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/events/qmlmouseevent.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/events/qmlqzobjects.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/events/qmlwheelevent.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/extensionscheme/qmlextensionscheme.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/extensionscheme/qmlwebengineurlrequestjob.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/fileutils/qmlfileutils.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/history/qmlhistory.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/history/qmlhistoryitem.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/i18n/qmli18n.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/menus/qmlaction.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/menus/qmlmenu.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/menus/qmlwebhittestresult.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/notifications/qmlnotifications.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/settings/qmlsettings.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/sidebar/qmlsidebar.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/tabs/qmltab.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/tabs/qmltabs.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/topsites/qmlmostvisitedurl.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/topsites/qmltopsites.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/userscript/qmlexternaljsobject.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/userscript/qmluserscript.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/userscript/qmluserscripts.h
        ${CMAKE_SOURCE_DIR}/src/lib/plugins/qml/api/windows/qmlwindow.h
        ${CMAKE_SOURCE_DIR}/src/lib/popupwindow/popuplocationbar.h
        ${CMAKE_SOURCE_DIR}/src/lib/popupwindow/popupstatusbarmessage.h
        ${CMAKE_SOURCE_DIR}/src/lib/popupwindow/popupwebview.h
        ${CMAKE_SOURCE_DIR}/src/lib/popupwindow/popupwindow.h
        ${CMAKE_SOURCE_DIR}/src/lib/preferences/acceptlanguage.h
        ${CMAKE_SOURCE_DIR}/src/lib/preferences/autofillmanager.h
        ${CMAKE_SOURCE_DIR}/src/lib/preferences/jsoptions.h
        ${CMAKE_SOURCE_DIR}/src/lib/preferences/pluginlistdelegate.h
        ${CMAKE_SOURCE_DIR}/src/lib/preferences/pluginsmanager.h
        ${CMAKE_SOURCE_DIR}/src/lib/preferences/preferences.h
        ${CMAKE_SOURCE_DIR}/src/lib/preferences/thememanager.h
        ${CMAKE_SOURCE_DIR}/src/lib/preferences/useragentdialog.h
        ${CMAKE_SOURCE_DIR}/src/lib/session/recoveryjsobject.h
        ${CMAKE_SOURCE_DIR}/src/lib/session/restoremanager.h
        ${CMAKE_SOURCE_DIR}/src/lib/session/sessionmanager.h
        ${CMAKE_SOURCE_DIR}/src/lib/session/sessionmanagerdialog.h
        ${CMAKE_SOURCE_DIR}/src/lib/sidebar/bookmarkssidebar.h
        ${CMAKE_SOURCE_DIR}/src/lib/sidebar/historysidebar.h
        ${CMAKE_SOURCE_DIR}/src/lib/sidebar/sidebar.h
        ${CMAKE_SOURCE_DIR}/src/lib/sidebar/sidebarinterface.h
        ${CMAKE_SOURCE_DIR}/src/lib/tabwidget/combotabbar.h
        ${CMAKE_SOURCE_DIR}/src/lib/tabwidget/tabbar.h
        ${CMAKE_SOURCE_DIR}/src/lib/tabwidget/tabcontextmenu.h
        ${CMAKE_SOURCE_DIR}/src/lib/tabwidget/tabicon.h
        ${CMAKE_SOURCE_DIR}/src/lib/tabwidget/tabmodel.h
        ${CMAKE_SOURCE_DIR}/src/lib/tabwidget/tabmrumodel.h
        ${CMAKE_SOURCE_DIR}/src/lib/tabwidget/tabstackedwidget.h
        ${CMAKE_SOURCE_DIR}/src/lib/tabwidget/tabtreemodel.h
        ${CMAKE_SOURCE_DIR}/src/lib/tabwidget/tabwidget.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/html5permissions/html5permissionsdialog.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/html5permissions/html5permissionsmanager.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/html5permissions/html5permissionsnotification.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/abstractbuttoninterface.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/aesinterface.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/animatedwidget.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/buttonwithmenu.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/certificateinfowidget.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/clickablelabel.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/closedtabsmanager.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/closedwindowsmanager.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/colors.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/delayedfilewatcher.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/desktopfile.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/docktitlebarwidget.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/enhancedmenu.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/focusselectlineedit.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/headerview.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/horizontallistwidget.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/iconprovider.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/listitemdelegate.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/mactoolbutton.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/menubar.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/pagethumbnailer.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/progressbar.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/qztools.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/removeitemfocusdelegate.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/scripts.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/sqldatabase.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/toolbutton.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/treewidget.h
        ${CMAKE_SOURCE_DIR}/src/lib/tools/wheelhelper.h
        ${CMAKE_SOURCE_DIR}/src/lib/webengine/javascript/autofilljsobject.h
        ${CMAKE_SOURCE_DIR}/src/lib/webengine/javascript/externaljsobject.h
        ${CMAKE_SOURCE_DIR}/src/lib/webengine/loadrequest.h
        ${CMAKE_SOURCE_DIR}/src/lib/webengine/webhittestresult.h
        ${CMAKE_SOURCE_DIR}/src/lib/webengine/webinspector.h
        ${CMAKE_SOURCE_DIR}/src/lib/webengine/webpage.h
        ${CMAKE_SOURCE_DIR}/src/lib/webengine/webscrollbar.h
        ${CMAKE_SOURCE_DIR}/src/lib/webengine/webscrollbarmanager.h
        ${CMAKE_SOURCE_DIR}/src/lib/webengine/webview.h
        ${CMAKE_SOURCE_DIR}/src/lib/webtab/searchtoolbar.h
        ${CMAKE_SOURCE_DIR}/src/lib/webtab/tabbedwebview.h
        ${CMAKE_SOURCE_DIR}/src/lib/webtab/webtab.h
    )

endif()
