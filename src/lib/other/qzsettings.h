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
#ifndef QZSETTINGS_H
#define QZSETTINGS_H

#include "qzcommon.h"
#include "settings.h"

#include <QStringList>

class MIDORI_EXPORT QzSettings
{
public:
    QzSettings();

    void loadSettings();
    void saveSettings();

    bool selectAllOnDoubleClick;
    bool selectAllOnClick;
    bool showLoadingProgress;
    int showLocationSuggestions;
    bool showSwitchTab;
    bool alwaysShowGoIcon;
    bool useInlineCompletion;
    bool searchOnEngineChange;
    bool searchFromAddressBar;
    bool searchWithDefaultEngine;
    bool showABSearchSuggestions;
    bool showWSBSearchSuggestions;
    QString activeTheme;
    int defaultZoomLevel;
    bool loadTabsOnActivation;
    bool httpsByDefault;
    QStringList autoOpenProtocols;
    QStringList blockedProtocols;
    bool localStorageEnabled;
    bool pluginsEnabled;
    bool javascriptEnabled;
    bool javascriptCanOpenWindows;
    bool javascriptCanAccessClipboard;
    bool linksIncludedInFocusChain;
    bool printElementBackgrounds;
    bool spatialNavigationEnabled;
    bool scrollAnimatorEnabled;
    bool hyperlinkAuditingEnabled;
    bool fullScreenSupportEnabled;
    bool localContentCanAccessRemoteUrls;
    bool localContentCanAccessFileUrls;
    bool screenCaptureEnabled;
    bool touchIconsEnabled;
    bool focusOnNavigationEnabled;
    bool allowJavaScriptActivateWindow;
    bool allowJavaScriptPaste;
    bool disableVideoAutoPlay;
    bool webRTCPublicIpOnly;
    QString standardFont;
    QString fixedFont;
    QString serifFont;
    QString sansSerifFont;
    QString cursiveFont;
    QString fantasyFont;
    QString pictographFont;
    QString cachePath;
    int defaultFontSize;
    int fixedFontSize;
    int minimumFontSize;
    int minimumLogicalFontSize;
    int afterlaunch;
    bool dNSPrefetch;
    bool intPDFViewer;
    bool checkDefaultBrowser;
    int wheelScrollLines;
    QString userStyleSheet;
    QString defaultEncoding;
    QUrl homepage;
    QUrl newTabUrl;
    bool newTabPos;
    Qz::NewTabPositionFlags newTabPosition;
    bool tabsOnTop;
    bool openPopupsInTabs;
    bool alwaysSwitchTabsWithWheel;
    bool checkUpdates;
    bool showStatusBar;
    bool instantBookmarksToolbar;
    bool showBookmarksToolbar;
    bool showNavigationToolbar;
    int settingsDialogPage;
    bool hideTabsWithOneTab;
    bool activateLastTabWhenClosingActual;
    bool newTabAfterActive;
    bool newEmptyTabAfterActive;
    bool dontCloseWithOneTab;
    bool askOnClosing;
    bool showClosedTabsButton;
    int showCloseOnInactiveTabs;
    int progressStyle;
    bool useCustomProgressColor;
    QColor customProgressColor;
    bool useNativeScrollbars;
    bool closeAppWithCtrlQ;
    bool allowLocalCache;
    bool deleteCacheOnClose;
    int localCacheSize;
    bool savePasswordsOnSites;
    bool autoCompletePasswords;
    bool allowHistory;
    bool deleteHistoryOnClose;
    bool deleteHTML5StorageOnClose;
    bool doNotTrack;
    QString defaultDownloadPath;
    bool closeManagerOnFinish;
    bool useExternalManager;
    QString externalManagerExecutable;
    QString externalManagerArguments;
    bool useTabNumberShortcuts;
    bool useSpeedDialNumberShortcuts;
    bool useSingleKeyShortcuts;
    int timeout;
    bool useNativeDesktop;
    QPoint position;
    bool enabled;
    QStringList languages;
    QString hostName;
    QString port;
    QString username;
    QString password;
    QByteArray geometry;
    bool allowAllUrlSchemes;
    bool userInteractUrlSchemes;
    bool allowPersistentCookies;
    bool notificationsEnabled;
    int proxyType;
    bool showMenuBar;
    QString defaultEngine;
    QString activeEngine;
    bool showSearchBar;
    QString pageHighlightColor;
    QString pageHighlightedTextColor;
    bool showNavInFullScreen;
    bool fullScreenLocationBarAutoHide;

private:

    bool resetPrefs;

};

#define qzSettings Settings::staticSettings()

#endif // QZSETTINGS_H
