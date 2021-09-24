/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2016  David Rosca <nowrep@gmail.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* ============================================================ */
#include "qzsettings.h"
#include "settings.h"
#include "webview.h"

QzSettings::QzSettings()
{
    loadSettings();
}

void QzSettings::loadSettings()
{
    Settings settings;

    settings.beginGroup(QStringLiteral("AddressBar"));
    selectAllOnDoubleClick = settings.value(QStringLiteral("SelectAllTextOnDoubleClick"), true).toBool();
    selectAllOnClick = settings.value(QStringLiteral("SelectAllTextOnClick"), true).toBool();
    showLoadingProgress = settings.value(QStringLiteral("ShowLoadingProgress"), true).toBool();
    showLocationSuggestions = settings.value(QStringLiteral("showSuggestions"), 2).toInt();
    showSwitchTab = settings.value(QStringLiteral("showSwitchTab"), true).toBool();
    alwaysShowGoIcon = settings.value(QStringLiteral("alwaysShowGoIcon"), false).toBool();
    useInlineCompletion = settings.value(QStringLiteral("useInlineCompletion"), true).toBool();
    progressStyle = settings.value(QStringLiteral("ProgressStyle"), 1).toInt();
    useCustomProgressColor = settings.value(QStringLiteral("UseCustomProgressColor"), true).toBool();
    customProgressColor = settings.value(QStringLiteral("CustomProgressColor"), QColor("#0078D7")).value<QColor>();
    settings.endGroup();

    settings.beginGroup(QStringLiteral("Browser-Fonts"));
#if defined(Q_OS_WIN)
    standardFont = settings.value(QStringLiteral("StandardFont"), QStringLiteral("Calibri")).toString();
    fixedFont = settings.value(QStringLiteral("FixedFont"), QStringLiteral("Consolas")).toString();
    serifFont = settings.value(QStringLiteral("SerifFont"), QStringLiteral("Cambria")).toString();
    sansSerifFont = settings.value(QStringLiteral("SansSerifFont"), QStringLiteral("Corbel")).toString();
    cursiveFont = settings.value(QStringLiteral("CursiveFont"), QStringLiteral("Segoe Print")).toString();
    fantasyFont = settings.value(QStringLiteral("FantasyFont"), QStringLiteral("Segoe UI Black")).toString();
    pictographFont = settings.value(QStringLiteral("PictographFont"), QStringLiteral("Segoe UI Symbol")).toString();
#elif defined(Q_OS_LINUX)
    standardFont = settings.value(QStringLiteral("StandardFont"), QStringLiteral("Liberation Sans")).toString();
    fixedFont = settings.value(QStringLiteral("FixedFont"), QStringLiteral("Liberation Mono")).toString();
    serifFont = settings.value(QStringLiteral("SerifFont"), QStringLiteral("Liberation Serif")).toString();
    sansSerifFont = settings.value(QStringLiteral("SansSerifFont"), QStringLiteral("Liberation Sans")).toString();
#endif
    defaultFontSize = settings.value(QStringLiteral("DefaultFontSize"), 14).toInt();
    fixedFontSize = settings.value(QStringLiteral("FixedFontSize"), 14).toInt();
    minimumFontSize = settings.value(QStringLiteral("MinimumFontSize"), 14).toInt();
    minimumLogicalFontSize = settings.value(QStringLiteral("MinimumLogicalFontSize"), 6).toInt();
    settings.endGroup();

    settings.beginGroup(QStringLiteral("Browser-Tabs-Settings"));
    newTabPos = settings.value("OpenNewTabsSelected", false).toBool();    
    newTabPosition = newTabPos ? Qz::NT_CleanSelectedTab : Qz::NT_CleanNotSelectedTab;
    tabsOnTop = settings.value(QStringLiteral("TabsOnTop"), false).toBool();
    openPopupsInTabs = settings.value(QStringLiteral("OpenPopupsInTabs"), true).toBool();
    alwaysSwitchTabsWithWheel = settings.value(QStringLiteral("AlwaysSwitchTabsWithWheel"), false).toBool();
    hideTabsWithOneTab = settings.value(QStringLiteral("hideTabsWithOneTab"), true).toBool();
    activateLastTabWhenClosingActual = settings.value(QStringLiteral("ActivateLastTabWhenClosingActual"), false).toBool();
    newTabAfterActive = settings.value(QStringLiteral("newTabAfterActive"), true).toBool();
    newEmptyTabAfterActive = settings.value(QStringLiteral("newEmptyTabAfterActive"), false).toBool();
    dontCloseWithOneTab = settings.value(QStringLiteral("dontCloseWithOneTab"), false).toBool();
    askOnClosing = settings.value(QStringLiteral("AskOnClosing"), false).toBool();
    showClosedTabsButton = settings.value(QStringLiteral("showClosedTabsButton"), false).toBool();
    showCloseOnInactiveTabs = settings.value(QStringLiteral("showCloseOnInactiveTabs"), 0).toInt();
    settings.endGroup();

    settings.beginGroup(QStringLiteral("Browser-View-Settings"));
    showStatusBar = settings.value(QStringLiteral("showStatusBar"), false).toBool();
    showMenuBar = settings.value(QStringLiteral("showMenubar"), false).toBool();
    instantBookmarksToolbar = settings.value(QStringLiteral("instantBookmarksToolbar"), false).toBool();
    showBookmarksToolbar = settings.value(QStringLiteral("showBookmarksToolbar"), false).toBool();
    showNavigationToolbar = settings.value(QStringLiteral("showNavigationToolbar"), true).toBool();
    settingsDialogPage = settings.value(QStringLiteral("settingsDialogPage"), 0).toInt();
    showNavInFullScreen = settings.value(QStringLiteral("ShowNavInFullscreen"), false).toBool();
    fullScreenLocationBarAutoHide = settings.value(QStringLiteral("AutoHideNavInFullscreen"), true).toBool();
    settings.endGroup();

    settings.beginGroup(QStringLiteral("DownloadManager"));
    defaultDownloadPath = settings.value(QStringLiteral("defaultDownloadPath"), "").toString();
    closeManagerOnFinish = settings.value(QStringLiteral("CloseManagerOnFinish"), false).toBool();
    useExternalManager = settings.value(QStringLiteral("UseExternalManager"), false).toBool();
    externalManagerExecutable = settings.value(QStringLiteral("ExternalManagerExecutable"), "").toString();
    externalManagerArguments = settings.value(QStringLiteral("ExternalManagerArguments"), "").toString();
    settings.endGroup();

    settings.beginGroup(QStringLiteral("NavigationBar"));
    showSearchBar = settings.value(QStringLiteral("ShowSearchBar"), false).toBool();
    settings.endGroup();

    settings.beginGroup(QStringLiteral("Notifications"));
    timeout = settings.value(QStringLiteral("Timeout"), 6000).toInt() / 1000;
#if defined(Q_OS_LINUX) && !defined(DISABLE_DBUS)
    useNativeDesktop = settings.value(QStringLiteral("UseNativeDesktop"), true).toBool();
#else
    useNativeDesktop = settings.value(QStringLiteral("UseNativeDesktop"), false).toBool();
#endif
    notificationsEnabled = settings.value(QStringLiteral("Enabled"), true).toBool();
    position = settings.value(QStringLiteral("Position"), QPoint(10, 10)).toPoint();
    settings.endGroup();

    settings.beginGroup(QStringLiteral("Preferences"));
    geometry = settings.value(QStringLiteral("Geometry")).toByteArray();
    settings.endGroup();

    settings.beginGroup(QStringLiteral("SearchEngines"));
    defaultEngine = settings.value("DefaultEngine", "DuckDuckGo").toString();
    activeEngine = settings.value("activeEngine", "DuckDuckGo").toString();
    searchOnEngineChange = settings.value(QStringLiteral("SearchOnEngineChange"), true).toBool();
    searchFromAddressBar = settings.value(QStringLiteral("SearchFromAddressBar"), true).toBool();
    searchWithDefaultEngine = settings.value(QStringLiteral("SearchWithDefaultEngine"), true).toBool();
    showABSearchSuggestions = settings.value(QStringLiteral("showSearchSuggestions"), true).toBool();
    showWSBSearchSuggestions = settings.value(QStringLiteral("showSuggestions"), true).toBool();
    settings.endGroup();

    settings.beginGroup(QStringLiteral("Shortcuts"));
    useTabNumberShortcuts = settings.value(QStringLiteral("useTabNumberShortcuts"), true).toBool();
    useSpeedDialNumberShortcuts = settings.value(QStringLiteral("useSpeedDialNumberShortcuts"), true).toBool();
    useSingleKeyShortcuts = settings.value(QStringLiteral("useSingleKeyShortcuts"), false).toBool();
    settings.endGroup();

    settings.beginGroup(QStringLiteral("SpellCheck"));
    enabled = settings.value(QStringLiteral("Enabled"), true).toBool();
    languages = settings.value(QStringLiteral("Languages"), QStringLiteral("en_GB")).toStringList();
    settings.endGroup();

    settings.beginGroup(QStringLiteral("Themes"));
    activeTheme = settings.value(QStringLiteral("activeTheme"), DEFAULT_THEME_NAME).toString();
    settings.endGroup();

    settings.beginGroup(QStringLiteral("Web-Browser-Settings"));
    useNativeScrollbars = settings.value(QStringLiteral("UseNativeScrollbars"), true).toBool();
    closeAppWithCtrlQ = settings.value(QStringLiteral("closeAppWithCtrlQ"), false).toBool();
    allowLocalCache = settings.value(QStringLiteral("AllowLocalCache"), true).toBool();
    deleteCacheOnClose = settings.value(QStringLiteral("deleteCacheOnClose"), true).toBool();
    localCacheSize = settings.value(QStringLiteral("LocalCacheSize"), 200).toInt();
    cachePath = settings.value(QStringLiteral("CachePath"), QString()).toString();
    savePasswordsOnSites = settings.value(QStringLiteral("SavePasswordsOnSites"), false).toBool();
    autoCompletePasswords = settings.value(QStringLiteral("AutoCompletePasswords"), false).toBool();
    allowHistory = settings.value(QStringLiteral("allowHistory"), true).toBool();
    deleteHistoryOnClose = settings.value(QStringLiteral("deleteHistoryOnClose"), false).toBool();
    deleteHTML5StorageOnClose = settings.value(QStringLiteral("deleteHTML5StorageOnClose"), true).toBool();
    doNotTrack = settings.value(QStringLiteral("DoNotTrack"), true).toBool();
    defaultZoomLevel = settings.value(QStringLiteral("DefaultZoomLevel"), WebView::zoomLevels().indexOf(100)).toInt();
    loadTabsOnActivation = settings.value(QStringLiteral("LoadTabsOnActivation"), true).toBool();
    autoOpenProtocols = settings.value(QStringLiteral("AutomaticallyOpenProtocols"), QStringList()).toStringList();
    blockedProtocols = settings.value(QStringLiteral("BlockOpeningProtocols"), QStringList()).toStringList();
    httpsByDefault = settings.value(QStringLiteral("httpsByDefault"), false).toBool();
    localStorageEnabled = settings.value(QStringLiteral("HTML5StorageEnabled"), true).toBool();
    pluginsEnabled = settings.value(QStringLiteral("allowPlugins"), true).toBool();
    javascriptEnabled = settings.value(QStringLiteral("allowJavaScript"), true).toBool();
    javascriptCanOpenWindows = settings.value(QStringLiteral("allowJavaScriptOpenWindow"), false).toBool();
    javascriptCanAccessClipboard = settings.value(QStringLiteral("allowJavaScriptAccessClipboard"), false).toBool();
    linksIncludedInFocusChain = settings.value(QStringLiteral("IncludeLinkInFocusChain"), false).toBool();
    printElementBackgrounds = settings.value(QStringLiteral("PrintElementBackground"), false).toBool();
    spatialNavigationEnabled = settings.value(QStringLiteral("SpatialNavigation"), true).toBool();
    scrollAnimatorEnabled = settings.value(QStringLiteral("AnimateScrolling"), true).toBool();
    hyperlinkAuditingEnabled = settings.value(QStringLiteral("HyperlinkAuditingEnabled"), false).toBool();
    fullScreenSupportEnabled = settings.value(QStringLiteral("FullScreenSupportEnabled"), true).toBool();
    localContentCanAccessRemoteUrls = settings.value(QStringLiteral("LocalContentCanAccessRemoteUrls"), true).toBool();
    localContentCanAccessFileUrls = settings.value(QStringLiteral("LocalContentCanAccessFileUrls"), true).toBool();
    screenCaptureEnabled = settings.value(QStringLiteral("ScreenCaptureEnabled"), false).toBool();
    touchIconsEnabled = settings.value(QStringLiteral("TouchIconsEnabled"), true).toBool();
    focusOnNavigationEnabled = settings.value(QStringLiteral("FocusOnNavigationEnabled"), false).toBool();
    allowJavaScriptActivateWindow = settings.value(QStringLiteral("allowJavaScriptActivateWindow"), false).toBool();
    allowJavaScriptPaste = settings.value(QStringLiteral("allowJavaScriptPaste"), false).toBool();
    disableVideoAutoPlay = settings.value(QStringLiteral("DisableVideoAutoPlay"), true).toBool();
    webRTCPublicIpOnly = settings.value(QStringLiteral("WebRTCPublicIpOnly"), true).toBool();
    dNSPrefetch = settings.value(QStringLiteral("DNSPrefetch"), true).toBool();
    intPDFViewer = settings.value(QStringLiteral("intPDFViewer"), true).toBool();
    defaultEncoding = settings.value(QStringLiteral("DefaultEncoding"), QStringLiteral("UTF-16")).toString();
    pageHighlightColor = settings.value(QStringLiteral("PageHighlightColor"), QLatin1String("#0078D7")).toString();
    pageHighlightedTextColor = settings.value(QStringLiteral("PageHighlightedTextColor"), QLatin1String("#fff")).toString();
    wheelScrollLines = settings.value(QStringLiteral("wheelScrollLines"), 5).toInt();
    userStyleSheet = settings.value(QStringLiteral("userStyleSheet"), QString()).toString();
#ifdef DISABLE_CHECK_UPDATES
    checkUpdates = settings.value(QStringLiteral("CheckUpdates"), false).toBool();
#else
    checkUpdates = settings.value(QStringLiteral("CheckUpdates"), true).toBool();
#endif
    allowAllUrlSchemes = settings.value(QStringLiteral("AllowAllUnknownUrlSchemes"), false).toBool();
    userInteractUrlSchemes = settings.value(QStringLiteral("UserInteractUrlSchemes"), false).toBool();
    allowPersistentCookies = settings.value(QStringLiteral("AllowPersistentCookies"), false).toBool();
#ifdef Q_OS_WIN
    checkDefaultBrowser = settings.value(QStringLiteral("CheckDefaultBrowser"), false).toBool();
#endif
    settings.endGroup();

    settings.beginGroup(QStringLiteral("Web-Proxy"));
    proxyType = settings.value(QStringLiteral("ProxyType"), 2).toInt();
    hostName = settings.value(QStringLiteral("HostName"), "").toString();
    port = settings.value(QStringLiteral("Port"), 8080).toString();
    username = settings.value(QStringLiteral("Username"), "").toString();
    password = settings.value(QStringLiteral("Password"), "").toString();
    settings.endGroup();

    settings.beginGroup(QStringLiteral("Web-URL-Settings"));
    homepage = settings.value(QStringLiteral("homepage"), QUrl(QStringLiteral("browser:start"))).toUrl();
    afterlaunch = settings.value(QStringLiteral("afterLaunch"), 3).toInt();
    newTabUrl = settings.value(QStringLiteral("newTabUrl"), QUrl(QStringLiteral("browser:speeddial"))).toUrl();
    settings.endGroup();

}

void QzSettings::saveSettings()
{
    Settings settings;

resetPrefs = settings.value(QStringLiteral("Web-Browser-Settings/resetPrefs"), false).toBool();
if (resetPrefs) {
    settings.clear();
    settings.setValue(QStringLiteral("Web-Browser-Settings/resetPrefs"), false);
} else {
    settings.beginGroup(QStringLiteral("AddressBar"));
    settings.setValue(QStringLiteral("SelectAllTextOnDoubleClick"), selectAllOnDoubleClick);
    settings.setValue(QStringLiteral("SelectAllTextOnClick"), selectAllOnClick);
    settings.setValue(QStringLiteral("ShowLoadingProgress"), showLoadingProgress);
    settings.setValue(QStringLiteral("showSuggestions"), showLocationSuggestions);
    settings.setValue(QStringLiteral("showSwitchTab"), showSwitchTab);
    settings.setValue(QStringLiteral("alwaysShowGoIcon"), alwaysShowGoIcon);
    settings.setValue(QStringLiteral("useInlineCompletion"), useInlineCompletion);
    settings.setValue(QStringLiteral("ProgressStyle"), progressStyle);
    settings.setValue(QStringLiteral("UseCustomProgressColor"), useCustomProgressColor);
    settings.setValue(QStringLiteral("CustomProgressColor"), customProgressColor);
    settings.endGroup();

    settings.beginGroup(QStringLiteral("Browser-Fonts"));
    if (!standardFont.isEmpty()) {
        settings.setValue(QStringLiteral("StandardFont"), standardFont);
    }
    if (!fixedFont.isEmpty()) {
        settings.setValue(QStringLiteral("FixedFont"), fixedFont);
    }
    if (!serifFont.isEmpty()) {
        settings.setValue(QStringLiteral("SerifFont"), serifFont);
    }
    if (!sansSerifFont.isEmpty()) {
        settings.setValue(QStringLiteral("SansSerifFont"), sansSerifFont);
    }
    if (!cursiveFont.isEmpty()) {
        settings.setValue(QStringLiteral("CursiveFont"), cursiveFont);
    }
    if (!fantasyFont.isEmpty()) {
        settings.setValue(QStringLiteral("FantasyFont"), fantasyFont);
    }
    if (!pictographFont.isEmpty()) {
        settings.setValue(QStringLiteral("PictographFont"), pictographFont);
    }
    settings.setValue(QStringLiteral("DefaultFontSize"), defaultFontSize);
    settings.setValue(QStringLiteral("FixedFontSize"), fixedFontSize);
    settings.setValue(QStringLiteral("MinimumFontSize"), minimumFontSize);
    settings.setValue(QStringLiteral("MinimumLogicalFontSize"), minimumLogicalFontSize);
    settings.endGroup();

    settings.beginGroup(QStringLiteral("Browser-Tabs-Settings"));
    settings.setValue(QStringLiteral("TabsOnTop"), tabsOnTop);
    settings.setValue(QStringLiteral("OpenNewTabsSelected"), newTabPos);
    settings.setValue(QStringLiteral("OpenPopupsInTabs"), openPopupsInTabs);
    settings.setValue(QStringLiteral("AlwaysSwitchTabsWithWheel"), alwaysSwitchTabsWithWheel);
    settings.setValue(QStringLiteral("hideTabsWithOneTab"), hideTabsWithOneTab);
    settings.setValue(QStringLiteral("ActivateLastTabWhenClosingActual"), activateLastTabWhenClosingActual);
    settings.setValue(QStringLiteral("newTabAfterActive"), newTabAfterActive);
    settings.setValue(QStringLiteral("newEmptyTabAfterActive"), newEmptyTabAfterActive);
    settings.setValue(QStringLiteral("dontCloseWithOneTab"), dontCloseWithOneTab);
    settings.setValue(QStringLiteral("AskOnClosing"), askOnClosing);
    settings.setValue(QStringLiteral("showClosedTabsButton"), showClosedTabsButton);
    settings.setValue(QStringLiteral("showCloseOnInactiveTabs"), showCloseOnInactiveTabs);
    settings.endGroup();

    settings.beginGroup(QStringLiteral("Browser-View-Settings"));
    settings.setValue(QStringLiteral("showStatusBar"), showStatusBar);
    settings.setValue(QStringLiteral("showMenubar"), showMenuBar);
    settings.setValue(QStringLiteral("instantBookmarksToolbar"), instantBookmarksToolbar);
    settings.setValue(QStringLiteral("showBookmarksToolbar"), showBookmarksToolbar);
    settings.setValue(QStringLiteral("showNavigationToolbar"), showNavigationToolbar);
    settings.setValue(QStringLiteral("settingsDialogPage"), settingsDialogPage);
    settings.setValue(QStringLiteral("ShowNavInFullscreen"), showNavInFullScreen);
    settings.setValue(QStringLiteral("AutoHideNavInFullscreen"), fullScreenLocationBarAutoHide);
    settings.endGroup();

    settings.beginGroup(QStringLiteral("DownloadManager"));
    settings.setValue(QStringLiteral("defaultDownloadPath"), defaultDownloadPath);
    settings.setValue(QStringLiteral("CloseManagerOnFinish"), closeManagerOnFinish);
    settings.setValue(QStringLiteral("UseExternalManager"), useExternalManager);
    settings.setValue(QStringLiteral("ExternalManagerExecutable"), externalManagerExecutable);
    settings.setValue(QStringLiteral("ExternalManagerArguments"), externalManagerArguments);
    settings.endGroup();

    settings.beginGroup(QStringLiteral("NavigationBar"));
    settings.setValue(QStringLiteral("ShowSearchBar"), showSearchBar);
    settings.endGroup();

    settings.beginGroup(QStringLiteral("Notifications"));
    settings.setValue(QStringLiteral("Timeout"), timeout * 1000);
    settings.setValue(QStringLiteral("UseNativeDesktop"), useNativeDesktop);
    settings.setValue(QStringLiteral("Enabled"), notificationsEnabled);
    settings.setValue(QStringLiteral("Position"), position);
    settings.endGroup();

    settings.beginGroup(QStringLiteral("Preferences"));
    settings.setValue(QStringLiteral("Geometry"), geometry);
    settings.endGroup();

    settings.beginGroup(QStringLiteral("SearchEngines"));
    settings.setValue(QStringLiteral("DefaultEngine"), defaultEngine);
    settings.setValue(QStringLiteral("activeEngine"), activeEngine);
    settings.setValue(QStringLiteral("SearchOnEngineChange"), searchOnEngineChange);
    settings.setValue(QStringLiteral("SearchFromAddressBar"), searchFromAddressBar);
    settings.setValue(QStringLiteral("SearchWithDefaultEngine"), searchWithDefaultEngine);
    settings.setValue(QStringLiteral("showSearchSuggestions"), showABSearchSuggestions);
    settings.setValue(QStringLiteral("showSuggestions"), showWSBSearchSuggestions);
    settings.endGroup();

    settings.beginGroup(QStringLiteral("Shortcuts"));
    settings.setValue(QStringLiteral("useTabNumberShortcuts"), useTabNumberShortcuts);
    settings.setValue(QStringLiteral("useSpeedDialNumberShortcuts"), useSpeedDialNumberShortcuts);
    settings.setValue(QStringLiteral("useSingleKeyShortcuts"), useSingleKeyShortcuts);
    settings.endGroup();

    settings.beginGroup(QStringLiteral("SpellCheck"));
    settings.setValue(QStringLiteral("Enabled"), enabled);
    settings.setValue(QStringLiteral("Languages"), languages);
    settings.endGroup();

    settings.beginGroup(QStringLiteral("Themes"));
    settings.setValue(QStringLiteral("activeTheme"), activeTheme);
    settings.endGroup();

    settings.beginGroup(QStringLiteral("Web-Browser-Settings"));
    settings.setValue(QStringLiteral("AutomaticallyOpenProtocols"), autoOpenProtocols);
    settings.setValue(QStringLiteral("BlockOpeningProtocols"), blockedProtocols);
    settings.setValue(QStringLiteral("UseNativeScrollbars"), useNativeScrollbars);
    settings.setValue(QStringLiteral("closeAppWithCtrlQ"), closeAppWithCtrlQ);
    settings.setValue(QStringLiteral("AllowLocalCache"), allowLocalCache);
    settings.setValue(QStringLiteral("deleteCacheOnClose"), deleteCacheOnClose);
    settings.setValue(QStringLiteral("LocalCacheSize"), localCacheSize);
    settings.setValue(QStringLiteral("CachePath"), cachePath);
    settings.setValue(QStringLiteral("SavePasswordsOnSites"), savePasswordsOnSites);
    settings.setValue(QStringLiteral("AutoCompletePasswords"), autoCompletePasswords);
    settings.setValue(QStringLiteral("allowHistory"), allowHistory);
    settings.setValue(QStringLiteral("deleteHistoryOnClose"), deleteHistoryOnClose);
    settings.setValue(QStringLiteral("deleteHTML5StorageOnClose"), deleteHTML5StorageOnClose);
    settings.setValue(QStringLiteral("DoNotTrack"), doNotTrack);
    settings.setValue(QStringLiteral("DefaultZoomLevel"), defaultZoomLevel);
    settings.setValue(QStringLiteral("LoadTabsOnActivation"), loadTabsOnActivation);
    settings.setValue(QStringLiteral("httpsByDefault"), httpsByDefault);
    settings.setValue(QStringLiteral("HTML5StorageEnabled"), localStorageEnabled);
    settings.setValue(QStringLiteral("allowPlugins"), pluginsEnabled);
    settings.setValue(QStringLiteral("allowJavaScript"), javascriptEnabled);
    settings.setValue(QStringLiteral("allowJavaScriptOpenWindow"), javascriptCanOpenWindows);
    settings.setValue(QStringLiteral("allowJavaScriptAccessClipboard"), javascriptCanAccessClipboard);
    settings.setValue(QStringLiteral("IncludeLinkInFocusChain"), linksIncludedInFocusChain);
    settings.setValue(QStringLiteral("PrintElementBackground"), printElementBackgrounds);
    settings.setValue(QStringLiteral("SpatialNavigation"), spatialNavigationEnabled);
    settings.setValue(QStringLiteral("AnimateScrolling"), scrollAnimatorEnabled);
    settings.setValue(QStringLiteral("HyperlinkAuditingEnabled"), hyperlinkAuditingEnabled);
    settings.setValue(QStringLiteral("FullScreenSupportEnabled"), fullScreenSupportEnabled);
    settings.setValue(QStringLiteral("LocalContentCanAccessRemoteUrls"), localContentCanAccessRemoteUrls);
    settings.setValue(QStringLiteral("LocalContentCanAccessFileUrls"), localContentCanAccessFileUrls);
    settings.setValue(QStringLiteral("ScreenCaptureEnabled"), screenCaptureEnabled);
    settings.setValue(QStringLiteral("TouchIconsEnabled"), touchIconsEnabled);
    settings.setValue(QStringLiteral("FocusOnNavigationEnabled"), focusOnNavigationEnabled);
    settings.setValue(QStringLiteral("allowJavaScriptActivateWindow"), allowJavaScriptActivateWindow);
    settings.setValue(QStringLiteral("allowJavaScriptPaste"), allowJavaScriptPaste);
    settings.setValue(QStringLiteral("DisableVideoAutoPlay"), disableVideoAutoPlay);
    settings.setValue(QStringLiteral("WebRTCPublicIpOnly"), webRTCPublicIpOnly);
    settings.setValue(QStringLiteral("DNSPrefetch"), dNSPrefetch);
    settings.setValue(QStringLiteral("intPDFViewer"), intPDFViewer);
    settings.setValue(QStringLiteral("DefaultEncoding"), defaultEncoding);
    settings.setValue(QStringLiteral("wheelScrollLines"), wheelScrollLines);
    settings.setValue(QStringLiteral("userStyleSheet"), userStyleSheet);
    settings.setValue(QStringLiteral("CheckUpdates"), checkUpdates);
    settings.setValue(QStringLiteral("AllowAllUnknownUrlSchemes"), allowAllUrlSchemes);
    settings.setValue(QStringLiteral("UserInteractUrlSchemes"), userInteractUrlSchemes);
    settings.setValue(QStringLiteral("AllowPersistentCookies"), allowPersistentCookies);
    settings.setValue(QStringLiteral("resetPrefs"), false);
#ifdef Q_OS_WIN
    settings.setValue(QStringLiteral("CheckDefaultBrowser"), checkDefaultBrowser);
#endif

    settings.endGroup();

    settings.beginGroup(QStringLiteral("Web-Proxy"));
    settings.setValue(QStringLiteral("HostName"), hostName);
    settings.setValue(QStringLiteral("Port"), port);
    settings.setValue(QStringLiteral("Username"), username);
    settings.setValue(QStringLiteral("Password"), password);
    settings.endGroup();

    settings.beginGroup(QStringLiteral("Web-URL-Settings"));
    settings.setValue(QStringLiteral("homepage"), homepage);
    settings.setValue(QStringLiteral("afterLaunch"), afterlaunch);
    settings.setValue(QStringLiteral("newTabUrl"), newTabUrl);
    settings.endGroup();
}
}
