/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2018 David Rosca <nowrep@gmail.com>
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
#include "mainapplication.h"
#include "history.h"
#include "qztools.h"
#include "updater.h"
#include "autofill.h"
#include "settings.h"
#include "autosaver.h"
#include "datapaths.h"
#include "tabwidget.h"
#include "cookiejar.h"
#include "bookmarks.h"
#include "qzsettings.h"
#include "proxystyle.h"
#include "pluginproxy.h"
#include "iconprovider.h"
#include "browserwindow.h"
#include "checkboxdialog.h"
#include "networkmanager.h"
#include "profilemanager.h"
#include "restoremanager.h"
#include "browsinglibrary.h"
#include "downloadmanager.h"
#include "clearprivatedata.h"
#include "useragentmanager.h"
#include "commandlineoptions.h"
#include "searchenginesmanager.h"
#include "desktopnotificationsfactory.h"
#include "html5permissions/html5permissionsmanager.h"
#include "scripts.h"
#include "sessionmanager.h"
#include "closedwindowsmanager.h"
#include "protocolhandlermanager.h"
#include "../config.h"

#include <QWebEngineSettings>
#include <QDesktopServices>
#include <QFontDatabase>
#include <QSqlDatabase>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QTranslator>
#include <QThreadPool>
#include <QSettings>
#include <QProcess>
#include <QTimer>
#include <QDir>
#include <QStandardPaths>
#include <QWebEngineProfile>
#include <QWebEngineDownloadItem>
#include <QWebEngineScriptCollection>
#include <QRegularExpression>
#include <QtWebEngineWidgetsVersion>
#include <QtWebEngineCoreVersion>

#include <QWebEngineNotification>

#ifdef Q_OS_WIN
#include <QtWin>
#include <QWinJumpList>
#include <QWinJumpListCategory>
#endif

#include <iostream>

#if defined(Q_OS_WIN) && !defined(Q_OS_OS2)
#include "registerqappassociation.h"
#include "lum_sdk.h"
#endif


static bool s_testMode = false;

MainApplication::MainApplication(int &argc, char** argv)
    : QtSingleApplication(argc, argv)
    , m_isPrivate(false)
    , m_isPortable(false)
    , m_isClosing(false)
    , m_isStartingAfterCrash(false)
    , m_history(nullptr)
    , m_bookmarks(nullptr)
    , m_autoFill(nullptr)
    , m_cookieJar(nullptr)
    , m_plugins(nullptr)
    , m_browsingLibrary(nullptr)
    , m_networkManager(nullptr)
    , m_restoreManager(nullptr)
    , m_sessionManager(nullptr)
    , m_downloadManager(nullptr)
    , m_userAgentManager(nullptr)
    , m_searchEnginesManager(nullptr)
    , m_closedWindowsManager(nullptr)
    , m_protocolHandlerManager(nullptr)
    , m_html5PermissionsManager(nullptr)
    , m_desktopNotifications(nullptr)
    , m_webProfile(nullptr)
    , m_autoSaver(nullptr)
#if defined(Q_OS_WIN) && !defined(Q_OS_OS2)
    , m_registerQAppAssociation(0)
#endif

#if defined(Q_OS_WIN)
    , lum_sdk_uninit();
#endif

#if defined (Q_OS_WIN)

        if (!lum_sdk_is_supported())
           {
            return;
           }
        lum_sdk_set_not_peer_txt(NOT_PEER_TXT_NOT_AGREE);
        lum_sdk_set_app_name("Midori Browser");
        lum_sdk_init_ui((char *)"win_midori-browser.org");

#endif
{
    setApplicationName(QStringLiteral("midori"));
    setOrganizationDomain(QStringLiteral("org.browser"));
    setWindowIcon(QIcon(QStringLiteral(":icons/midori-green.svg")));
    setDesktopFileName(QStringLiteral("Midori Browser"));

#ifdef GIT_REVISION
    setApplicationVersion(QStringLiteral("%1 (%2)").arg(Qz::VERSION, GIT_REVISION));
#else
    setApplicationVersion(QString::fromLatin1(Qz::VERSION));
#endif
    // QSQLITE database plugin is required
    if (!QSqlDatabase::isDriverAvailable(QStringLiteral("QSQLITE"))) {
        QMessageBox::critical(nullptr, QStringLiteral("Error"), QStringLiteral("Qt SQLite database plugin is not available. Please install it and restart the application."));
        m_isClosing = true;
        return;
    }

#ifdef Q_OS_WIN
    // Set default app font (needed for N'ko)
    int fontId = QFontDatabase::addApplicationFont(QStringLiteral("font.ttf"));
    if (fontId != -1) {
        const QStringList families = QFontDatabase::applicationFontFamilies(fontId);
        if (!families.empty())
            setFont(QFont(families.at(0)));
    }
#endif

    QUrl startUrl;
    QString startProfile;
    QStringList messages;

    bool noAddons = false;
    bool newInstance = false;

    if (argc > 1) {
        CommandLineOptions cmd;
        const QVector<CommandLineOptions::ActionPair> actions = cmd.getActions();
        for (const CommandLineOptions::ActionPair &pair : actions) {
            switch (pair.action) {
            case Qz::CL_StartWithoutAddons:
                noAddons = true;
                break;
            case Qz::CL_StartWithProfile:
                startProfile = pair.text;
                break;
            case Qz::CL_StartPortable:
                m_isPortable = true;
                break;
            case Qz::CL_NewTab:
                messages.append(QStringLiteral("ACTION:NewTab"));
                m_postLaunchActions.append(OpenNewTab);
                break;
            case Qz::CL_NewWindow:
                messages.append(QStringLiteral("ACTION:NewWindow"));
                break;
            case Qz::CL_ToggleFullScreen:
                messages.append(QStringLiteral("ACTION:ToggleFullScreen"));
                m_postLaunchActions.append(ToggleFullScreen);
                break;
            case Qz::CL_ShowDownloadManager:
                messages.append(QStringLiteral("ACTION:ShowDownloadManager"));
                m_postLaunchActions.append(OpenDownloadManager);
                break;
            case Qz::CL_StartPrivateBrowsing:
                m_isPrivate = true;
                break;
            case Qz::CL_StartNewInstance:
                newInstance = true;
                break;
            case Qz::CL_OpenUrlInCurrentTab:
                startUrl = QUrl::fromUserInput(pair.text);
                messages.append("ACTION:OpenUrlInCurrentTab" + pair.text);
                break;
            case Qz::CL_OpenUrlInNewWindow:
                startUrl = QUrl::fromUserInput(pair.text);
                messages.append("ACTION:OpenUrlInNewWindow" + pair.text);
                break;
            case Qz::CL_OpenUrl:
                startUrl = QUrl::fromUserInput(pair.text);
                messages.append("URL:" + pair.text);
                break;
            case Qz::CL_ExitAction:
                m_isClosing = true;
                return;
            case Qz::CL_WMClass:
                m_wmClass = pair.text.toUtf8();
                break;
            default:
                break;
            }
        }
    }

    if (!isPortable()) {
        QSettings bhawkConf(QStringLiteral("%1/midori.conf").arg(applicationDirPath()), QSettings::IniFormat);
        m_isPortable = bhawkConf.value(QStringLiteral("Config/Portable")).toBool();
    }

    if (isPortable()) {
        std::cout << "Midori Browser: Running in Portable Mode." << std::endl;
        DataPaths::setPortableVersion();
    }
    // Don't start single application in private browsing
    if (!isPrivate()) {
        QString appId = QStringLiteral("org.browser.bhawk");

        if (isPortable()) {
            appId.append(QLatin1String(".Portable"));
        }

        if (isTestModeEnabled()) {
            appId.append(QStringLiteral(".TestMode"));
        }

        if (newInstance) {
            if (startProfile.isEmpty() || startProfile == QLatin1String("default")) {
                std::cout << "New instance cannot be started with default profile!" << std::endl;
            }
            else {
                // Generate unique appId so it is possible to start more separate instances
                // of the same profile. It is dangerous to run more instances of the same profile,
                // but if the user wants it, we should allow it.
                appId.append(QLatin1Char('.') + startProfile + QString::number(QDateTime::currentMSecsSinceEpoch()));
            }
        }

        setAppId(appId);
    }

    // If there is nothing to tell other instance, we need to at least wake it
    if (messages.isEmpty()) {
        messages.append(QStringLiteral(" "));
    }

    if (isRunning()) {
        m_isClosing = true;
        for (const QString &message : qAsConst(messages)) {
            sendMessage(message);
        }
        return;
    }

#ifdef Q_OS_MACOS
    setQuitOnLastWindowClosed(false);
    // disable tabbing issue#2261
    extern void disableWindowTabbing();
    disableWindowTabbing();
#else
    setQuitOnLastWindowClosed(true);
#endif

    QSettings::setDefaultFormat(QSettings::IniFormat);
    QDesktopServices::setUrlHandler(QStringLiteral("http"), this, "addNewTab");
    QDesktopServices::setUrlHandler(QStringLiteral("https"), this, "addNewTab");
    QDesktopServices::setUrlHandler(QStringLiteral("ftp"), this, "addNewTab");

    ProfileManager profileManager;
    profileManager.initConfigDir();
    profileManager.initCurrentProfile(startProfile);

    if (!QFile::exists(DataPaths::currentProfilePath() + QLatin1String("/settings.ini"))) {
        #define FIRST_EVER_RUN true
    }

    Settings::createSettings(DataPaths::currentProfilePath() + QLatin1String("/settings.ini"));

    NetworkManager::registerSchemes();

    m_webProfile = isPrivate() ? new QWebEngineProfile() : QWebEngineProfile::defaultProfile();
    connect(m_webProfile, &QWebEngineProfile::downloadRequested, this, &MainApplication::downloadRequested);

    m_webProfile->setNotificationPresenter([&] (std::unique_ptr<QWebEngineNotification> notification) {
        DesktopNotificationsFactory * notifications = desktopNotifications();
        notifications->showNotification(
            QPixmap::fromImage(notification->icon()), notification->title(), notification->message()
        );
    });

    m_networkManager = new NetworkManager(this);

    setupUserScripts();

    if (!isPrivate() && !isTestModeEnabled()) {
        m_sessionManager = new SessionManager(this);
        m_autoSaver = new AutoSaver(this);
        connect(m_autoSaver, &AutoSaver::save, m_sessionManager, &SessionManager::autoSaveLastSession);

        Settings settings;
        settings.beginGroup(QStringLiteral("SessionRestore"));
        const bool wasRunning = settings.value(QStringLiteral("isRunning"), false).toBool();
        const bool wasRestoring = settings.value(QStringLiteral("isRestoring"), false).toBool();
        settings.setValue(QStringLiteral("isRunning"), true);
        settings.setValue(QStringLiteral("isRestoring"), wasRunning);
        settings.endGroup();
        settings.sync();

        m_isStartingAfterCrash = wasRunning && wasRestoring;

        if (wasRunning) {
            QTimer::singleShot(60 * 1000, this, [this]() {
                Settings().setValue(QStringLiteral("SessionRestore/isRestoring"), false);
            });
        }

        // we have to ask about startup session before creating main window
        if (!m_isStartingAfterCrash && afterLaunch() == SelectSession)
            m_restoreManager = new RestoreManager(sessionManager()->askSessionFromUser());
    }

    BrowserWindow* window = createWindow(Qz::BW_FirstAppWindow, startUrl);

    loadSettings();

    m_plugins = new PluginProxy(this);
    m_autoFill = new AutoFill(this);
    mApp->protocolHandlerManager();

    if (!noAddons)
        m_plugins->loadPlugins();

    connect(window, SIGNAL(startingCompleted()), this, SLOT(restoreOverrideCursor()));
    connect(this, &QApplication::focusChanged, this, &MainApplication::onFocusChanged);

    if (!isPrivate() && !isTestModeEnabled()) {
        Settings settings;
#ifndef DISABLE_CHECK_UPDATES
        checkUpdates = settings.value("Web-Browser-Settings/CheckUpdates", true).toBool();

        if (checkUpdates) {
            new Updater(window);
        }
#else
        checkUpdates = settings.value("Web-Browser-Settings/CheckUpdates", false).toBool();
#endif
        sessionManager()->backupSavedSessions();

        if (m_isStartingAfterCrash || afterLaunch() == RestoreSession) {
            m_restoreManager = new RestoreManager(sessionManager()->lastActiveSessionPath());
            if (!m_restoreManager->isValid()) {
                destroyRestoreManager();
            }
        }

        if (!m_isStartingAfterCrash && m_restoreManager) {
            restoreSession(window, m_restoreManager->restoreData());
        }
    }

    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, DataPaths::currentProfilePath());

    connect(this, SIGNAL(messageReceived(QString)), this, SLOT(messageReceived(QString)));
    connect(this, &QCoreApplication::aboutToQuit, this, &MainApplication::saveSettings);

    QTimer::singleShot(0, this, &MainApplication::postLaunch);
}

MainApplication::~MainApplication()
{
    m_isClosing = true;

    IconProvider::instance()->saveIconsToDatabase();

    // Wait for all QtConcurrent jobs to finish
    QThreadPool::globalInstance()->waitForDone();

    // Delete all classes that are saving data in destructor
    delete m_bookmarks;
    m_bookmarks = nullptr;
    delete m_cookieJar;
    m_cookieJar = nullptr;

    Settings::syncSettings();
}

bool MainApplication::isClosing() const
{
    return m_isClosing;
}

bool MainApplication::isPrivate() const
{
    return m_isPrivate;
}

bool MainApplication::isPortable() const
{
#ifdef PORTABLE_BUILD
    return true;
#else
    return m_isPortable;
#endif
}

bool MainApplication::isStartingAfterCrash() const
{
    return m_isStartingAfterCrash;
}

int MainApplication::windowCount() const
{
    return m_windows.count();
}

QList<BrowserWindow*> MainApplication::windows() const
{
    return m_windows;
}

BrowserWindow* MainApplication::getWindow() const
{
    if (m_lastActiveWindow) {
        return m_lastActiveWindow.data();
    }

    return m_windows.isEmpty() ? 0 : m_windows.at(0);
}

BrowserWindow* MainApplication::createWindow(Qz::BrowserWindowType type, const QUrl &startUrl)
{
    if (windowCount() == 0 && type != Qz::BW_MacFirstWindow) {
        type = Qz::BW_FirstAppWindow;
    }

    BrowserWindow* window = new BrowserWindow(type, startUrl);
    connect(window, &QObject::destroyed, this, &MainApplication::windowDestroyed);

    m_windows.prepend(window);
    return window;
}

MainApplication::AfterLaunch MainApplication::afterLaunch() const
{
    return static_cast<AfterLaunch>(Settings().value(QStringLiteral("Web-URL-Settings/afterLaunch"), RestoreSession).toInt());
}

void MainApplication::openSession(BrowserWindow* window, RestoreData &restoreData)
{
    setOverrideCursor(Qt::BusyCursor);

    if (!window)
        window = createWindow(Qz::BW_OtherRestoredWindow);

    if (window->tabCount() != 0) {
        // This can only happen when recovering crashed session!
        // Don't restore tabs in current window as user already opened some new tabs.
        createWindow(Qz::BW_OtherRestoredWindow)->restoreWindow(restoreData.windows.takeAt(0));
    } else {
        window->restoreWindow(restoreData.windows.takeAt(0));
    }

    const QVector<BrowserWindow::SavedWindow> restoreWindows = restoreData.windows;
    for (const BrowserWindow::SavedWindow &data : restoreWindows) {
        BrowserWindow* window = createWindow(Qz::BW_OtherRestoredWindow);
        window->restoreWindow(data);
    }

    m_closedWindowsManager->restoreState(restoreData.closedWindows);

    restoreOverrideCursor();
}

bool MainApplication::restoreSession(BrowserWindow* window, RestoreData restoreData)
{
    if (m_isPrivate || !restoreData.isValid()) {
        return false;
    }

    openSession(window, restoreData);

    m_restoreManager->clearRestoreData();
    destroyRestoreManager();

    return true;
}

void MainApplication::destroyRestoreManager()
{
    if (m_restoreManager && m_restoreManager->isValid()) {
        return;
    }

    delete m_restoreManager;
    m_restoreManager = 0;
}

void MainApplication::reloadSettings()
{
    loadSettings();
    emit settingsReloaded();
}

QString MainApplication::styleName() const
{
    return m_proxyStyle ? m_proxyStyle->name() : QString();
}

void MainApplication::setProxyStyle(ProxyStyle *style)
{
    m_proxyStyle = style;
    setStyle(style);
}

QByteArray MainApplication::wmClass() const
{
    return m_wmClass;
}

History* MainApplication::history()
{
    if (!m_history) {
        m_history = new History(this);
    }
    return m_history;
}

Bookmarks* MainApplication::bookmarks()
{
    if (!m_bookmarks) {
        m_bookmarks = new Bookmarks(this);
    }
    return m_bookmarks;
}

AutoFill* MainApplication::autoFill()
{
    return m_autoFill;
}

CookieJar* MainApplication::cookieJar()
{
    if (!m_cookieJar) {
        m_cookieJar = new CookieJar(this);
    }
    return m_cookieJar;
}

PluginProxy* MainApplication::plugins()
{
    return m_plugins;
}

BrowsingLibrary* MainApplication::browsingLibrary()
{
    if (!m_browsingLibrary) {
        m_browsingLibrary = new BrowsingLibrary(getWindow());
    }
    return m_browsingLibrary;
}

NetworkManager *MainApplication::networkManager()
{
    return m_networkManager;
}

RestoreManager* MainApplication::restoreManager()
{
    return m_restoreManager;
}

SessionManager* MainApplication::sessionManager()
{
    return m_sessionManager;
}

DownloadManager* MainApplication::downloadManager()
{
    if (!m_downloadManager) {
        m_downloadManager = new DownloadManager();
    }
    return m_downloadManager;
}

UserAgentManager* MainApplication::userAgentManager()
{
    if (!m_userAgentManager) {
        m_userAgentManager = new UserAgentManager(this);
    }
    return m_userAgentManager;
}

SearchEnginesManager* MainApplication::searchEnginesManager()
{
    if (!m_searchEnginesManager) {
        m_searchEnginesManager = new SearchEnginesManager(this);
    }
    return m_searchEnginesManager;
}

ClosedWindowsManager* MainApplication::closedWindowsManager()
{
    if (!m_closedWindowsManager) {
        m_closedWindowsManager = new ClosedWindowsManager(this);
    }
    return m_closedWindowsManager;
}

ProtocolHandlerManager *MainApplication::protocolHandlerManager()
{
    if (!m_protocolHandlerManager) {
        m_protocolHandlerManager = new ProtocolHandlerManager(this);
    }
    return m_protocolHandlerManager;
}

HTML5PermissionsManager* MainApplication::html5PermissionsManager()
{
    if (!m_html5PermissionsManager) {
        m_html5PermissionsManager = new HTML5PermissionsManager(this);
    }
    return m_html5PermissionsManager;
}

DesktopNotificationsFactory* MainApplication::desktopNotifications()
{
    if (!m_desktopNotifications) {
        m_desktopNotifications = new DesktopNotificationsFactory(this);
    }
    return m_desktopNotifications;
}

QWebEngineProfile *MainApplication::webProfile() const
{
    return m_webProfile;
}

QWebEngineSettings *MainApplication::webSettings() const
{
    return m_webProfile->settings();
}

// static
MainApplication* MainApplication::instance()
{
    return static_cast<MainApplication*>(QCoreApplication::instance());
}

// static
bool MainApplication::isTestModeEnabled()
{
    return s_testMode;
}

// static
void MainApplication::setTestModeEnabled(bool enabled)
{
    s_testMode = enabled;
}

void MainApplication::addNewTab(const QUrl &url)
{
    BrowserWindow* window = getWindow();

    if (window) {
        window->tabWidget()->addView(url, url.isEmpty() ? Qz::NT_SelectedNewEmptyTab : Qz::NT_SelectedTabAtTheEnd);
    }
}

void MainApplication::startPrivateBrowsing(const QUrl &startUrl)
{
    QUrl url = startUrl;
    if (QAction* act = qobject_cast<QAction*>(sender())) {
        url = act->data().toUrl();
    }

    QStringList args;
    args.append(QStringLiteral("--opt-c"));
    args.append(QStringLiteral("--opt-a=") + ProfileManager::currentProfile());

    if (!url.isEmpty()) {
        args << url.toEncoded();
    }

    if (!QProcess::startDetached(applicationFilePath(), args)) {
        qWarning() << "MainApplication: Cannot start new browser process for private browsing!" << applicationFilePath() << args;
    }
}

void MainApplication::reloadUserStyleSheet()
{
    const QString userCssFile = Settings().value(QStringLiteral("Web-Browser-Settings/userStyleSheet"), QString()).toString();
    setUserStyleSheet(userCssFile);
}

void MainApplication::restoreOverrideCursor()
{
    QApplication::restoreOverrideCursor();
}

void MainApplication::changeOccurred()
{
    if (m_autoSaver)
        m_autoSaver->changeOccurred();
}

void MainApplication::quitApplication()
{
#ifdef Q_OS_LINUX
    if (!plat.isEmpty()) {
        qputenv("QT_QPA_PLATFORM", plat);
    }
#endif
    if (m_downloadManager && !m_downloadManager->canClose()) {
        m_downloadManager->show();
        return;
    }

    for (BrowserWindow *window : qAsConst(m_windows)) {
        emit window->aboutToClose();
    }

    if (m_sessionManager && m_windows.count() > 0) {
        m_sessionManager->autoSaveLastSession();
    }

    m_isClosing = true;

    for (BrowserWindow *window : qAsConst(m_windows)) {
        window->close();
    }

    // Saving settings in saveSettings() slot called from quit() so
    // everything gets saved also when quitting application in other
    // way than clicking Quit action in File menu or closing last window
    // eg. on Mac (#157)

    if (!isPrivate()) {
        removeLockFile();
    }

    quit();
}

void MainApplication::postLaunch()
{
    if (m_postLaunchActions.contains(OpenDownloadManager)) {
        downloadManager()->show();
    }

    if (m_postLaunchActions.contains(OpenNewTab)) {
        getWindow()->tabWidget()->addView(QUrl(), Qz::NT_SelectedNewEmptyTab);
    }

    if (m_postLaunchActions.contains(ToggleFullScreen)) {
        //getWindow()->toggleFullScreen();
        QTimer::singleShot(1600, getWindow(), &BrowserWindow::toggleFullScreen);
    }

    createJumpList();
#ifdef Q_OS_LINUX
    initPulseSupport();
#endif

    QTimer::singleShot(5000, this, &MainApplication::runDeferredPostLaunchActions);
}

QByteArray MainApplication::saveState() const
{
    RestoreData restoreData;
    restoreData.windows.reserve(m_windows.count());
    for (BrowserWindow *window : qAsConst(m_windows)) {
        restoreData.windows.append(BrowserWindow::SavedWindow(window));
    }

    if (m_restoreManager && m_restoreManager->isValid()) {
        QDataStream stream(&restoreData.crashedSession, QIODevice::WriteOnly);
        stream << m_restoreManager->restoreData();
    }

    restoreData.closedWindows = m_closedWindowsManager->saveState();

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream << Qz::sessionVersion;
    stream << restoreData;

    return data;
}

void MainApplication::saveSettings()
{
    if (isPrivate()) {
        return;
    }

    m_isClosing = true; // Should this not be a signal???

    Settings settings;
    settings.beginGroup(QStringLiteral("SessionRestore"));
    settings.setValue(QStringLiteral("isRunning"), false);
    settings.setValue(QStringLiteral("isRestoring"), false);
    settings.endGroup();

    settings.beginGroup(QStringLiteral("Web-Browser-Settings"));
    bool deleteCache = settings.value(QStringLiteral("deleteCacheOnClose"), true).toBool();
    bool deleteHistory = settings.value(QStringLiteral("deleteHistoryOnClose"), false).toBool();
    bool deleteHtml5Storage = settings.value(QStringLiteral("deleteHTML5StorageOnClose"), true).toBool();
    settings.endGroup();

    settings.beginGroup(QStringLiteral("Cookie-Settings"));
    bool deleteCookies = settings.value(QStringLiteral("deleteCookiesOnClose"), false).toBool();
    settings.endGroup();

    if (deleteHistory) {
        m_history->clearHistory();
    }
    if (deleteHtml5Storage) {
        ClearPrivateData::clearLocalStorage();
    }
    if (deleteCookies) {
        m_cookieJar->deleteAllCookies(false);
    }
    if (deleteCache) {
        QzTools::removeRecursively(mApp->webProfile()->cachePath());
    }

    m_searchEnginesManager->saveSettings();
    m_plugins->shutdown();
    m_networkManager->shutdown();

    qzSettings->saveSettings();
    QFile::remove(DataPaths::currentProfilePath() + QLatin1String("/WebpageIcons.db"));

    sessionManager()->saveSettings();
}

void MainApplication::messageReceived(const QString &message)
{
    QWidget* actWin = getWindow();
    QUrl actUrl;

    if (message.startsWith(QLatin1String("URL:"))) {
        const QUrl url = QUrl::fromUserInput(message.mid(4));
        addNewTab(url);
        actWin = getWindow();
    }
    else if (message.startsWith(QLatin1String("ACTION:"))) {
        const QString text = message.mid(7);
        if (text == QLatin1String("NewTab")) {
            addNewTab();
        }
        else if (text == QLatin1String("NewWindow")) {
            actWin = createWindow(Qz::BW_NewWindow);
        }
        else if (text == QLatin1String("ShowDownloadManager")) {
            downloadManager()->show();
            actWin = downloadManager();
        }
        else if (text == QLatin1String("ToggleFullScreen") && actWin) {
            BrowserWindow* qz = static_cast<BrowserWindow*>(actWin);
            qz->toggleFullScreen();
        }
        else if (text.startsWith(QLatin1String("OpenUrlInCurrentTab"))) {
            actUrl = QUrl::fromUserInput(text.mid(19));
        }
        else if (text.startsWith(QLatin1String("OpenUrlInNewWindow"))) {
            createWindow(Qz::BW_NewWindow, QUrl::fromUserInput(text.mid(18)));
            return;
        }
    }
    else {
        // User attempted to start another instance, let's open a new window
        actWin = createWindow(Qz::BW_NewWindow);
    }

    if (!actWin) {
        if (!isClosing()) {
            // It can only occur if download manager window was still opened
            createWindow(Qz::BW_NewWindow, actUrl);
        }
        return;
    }

    actWin->setWindowState(actWin->windowState() & ~Qt::WindowMinimized);
    actWin->raise();
    actWin->activateWindow();
    actWin->setFocus();

    BrowserWindow* win = qobject_cast<BrowserWindow*>(actWin);

    if (win && !actUrl.isEmpty()) {
        win->loadAddress(actUrl);
    }
}

void MainApplication::windowDestroyed(QObject* window)
{
    // qobject_cast doesn't work because QObject::destroyed is emitted from destructor
    Q_ASSERT(static_cast<BrowserWindow*>(window));
    Q_ASSERT(m_windows.contains(static_cast<BrowserWindow*>(window)));

    m_windows.removeOne(static_cast<BrowserWindow*>(window));
}

void MainApplication::onFocusChanged()
{
    BrowserWindow* activeBrowserWindow = qobject_cast<BrowserWindow*>(activeWindow());

    if (activeBrowserWindow) {
        m_lastActiveWindow = activeBrowserWindow;

        emit activeWindowChanged(m_lastActiveWindow);
    }
}

void MainApplication::runDeferredPostLaunchActions()
{
    checkOptimizeDatabase();
}

void MainApplication::downloadRequested(QWebEngineDownloadItem *download)
{
    downloadManager()->download(download);
}

void MainApplication::loadSettings()
{
    Settings settings;

    QWebEngineSettings* webSettings = m_webProfile->settings();

    if (isPrivate()) {
        webSettings->setAttribute(QWebEngineSettings::LocalStorageEnabled, false);
        history()->setSaving(false);
    }

    if (m_downloadManager) {
        m_downloadManager->loadSettings();
    }

    qzSettings->loadSettings();
    userAgentManager()->loadSettings();
    networkManager()->loadSettings();
    // Web browsing settings // https://doc.qt.io/qt-5/qwebenginesettings.html
    webSettings->setAttribute(QWebEngineSettings::LocalStorageEnabled, qzSettings->localStorageEnabled);
    webSettings->setAttribute(QWebEngineSettings::PluginsEnabled, qzSettings->pluginsEnabled);
    webSettings->setAttribute(QWebEngineSettings::JavascriptEnabled, qzSettings->javascriptEnabled);
    webSettings->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, qzSettings->javascriptCanOpenWindows);
    webSettings->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, qzSettings->javascriptCanAccessClipboard);
    webSettings->setAttribute(QWebEngineSettings::LinksIncludedInFocusChain, qzSettings->linksIncludedInFocusChain);
    webSettings->setAttribute(QWebEngineSettings::PrintElementBackgrounds, qzSettings->printElementBackgrounds);
    webSettings->setAttribute(QWebEngineSettings::SpatialNavigationEnabled, qzSettings->spatialNavigationEnabled);
    webSettings->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, qzSettings->scrollAnimatorEnabled);
    webSettings->setAttribute(QWebEngineSettings::HyperlinkAuditingEnabled, qzSettings->hyperlinkAuditingEnabled);
    webSettings->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, qzSettings->fullScreenSupportEnabled);
    webSettings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, qzSettings->localContentCanAccessRemoteUrls);
    webSettings->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, qzSettings->localContentCanAccessFileUrls);
    webSettings->setAttribute(QWebEngineSettings::ScreenCaptureEnabled, qzSettings->screenCaptureEnabled);
    webSettings->setAttribute(QWebEngineSettings::TouchIconsEnabled, qzSettings->touchIconsEnabled);
    webSettings->setAttribute(QWebEngineSettings::FocusOnNavigationEnabled, qzSettings->focusOnNavigationEnabled);
    webSettings->setAttribute(QWebEngineSettings::AllowWindowActivationFromJavaScript, qzSettings->allowJavaScriptActivateWindow);
    webSettings->setAttribute(QWebEngineSettings::JavascriptCanPaste, qzSettings->allowJavaScriptPaste);
    webSettings->setAttribute(QWebEngineSettings::PlaybackRequiresUserGesture, qzSettings->disableVideoAutoPlay);
    webSettings->setAttribute(QWebEngineSettings::WebRTCPublicInterfacesOnly, qzSettings->webRTCPublicIpOnly);
    webSettings->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, qzSettings->dNSPrefetch);
    webSettings->setAttribute(QWebEngineSettings::PdfViewerEnabled, qzSettings->intPDFViewer);

    webSettings->setDefaultTextEncoding(qzSettings->defaultEncoding);

    setWheelScrollLines(qzSettings->wheelScrollLines);

    webSettings->setFontFamily(QWebEngineSettings::StandardFont, settings.value(QStringLiteral("Browser-Fonts/StandardFont"), webSettings->fontFamily(QWebEngineSettings::StandardFont)).toString());
    webSettings->setFontFamily(QWebEngineSettings::FixedFont, settings.value(QStringLiteral("Browser-Fonts/FixedFont"), webSettings->fontFamily(QWebEngineSettings::FixedFont)).toString());
    webSettings->setFontFamily(QWebEngineSettings::SerifFont, settings.value(QStringLiteral("Browser-Fonts/SerifFont"), webSettings->fontFamily(QWebEngineSettings::SerifFont)).toString());
    webSettings->setFontFamily(QWebEngineSettings::SansSerifFont, settings.value(QStringLiteral("Browser-Fonts/SansSerifFont"), webSettings->fontFamily(QWebEngineSettings::SansSerifFont)).toString());
    webSettings->setFontFamily(QWebEngineSettings::CursiveFont, settings.value(QStringLiteral("Browser-Fonts/CursiveFont"), webSettings->fontFamily(QWebEngineSettings::CursiveFont)).toString());
    webSettings->setFontFamily(QWebEngineSettings::FantasyFont, settings.value(QStringLiteral("Browser-Fonts/FantasyFont"), webSettings->fontFamily(QWebEngineSettings::FantasyFont)).toString());
    webSettings->setFontFamily(QWebEngineSettings::PictographFont, settings.value(QStringLiteral("Browser-Fonts/PictographFont"), webSettings->fontFamily(QWebEngineSettings::PictographFont)).toString());
    webSettings->setFontSize(QWebEngineSettings::DefaultFontSize, qzSettings->defaultFontSize);
    webSettings->setFontSize(QWebEngineSettings::DefaultFixedFontSize, qzSettings->fixedFontSize);
    webSettings->setFontSize(QWebEngineSettings::MinimumFontSize, qzSettings->minimumFontSize);
    webSettings->setFontSize(QWebEngineSettings::MinimumLogicalFontSize, qzSettings->minimumLogicalFontSize);

    const bool userInteractUrlSchemes = qzSettings->userInteractUrlSchemes;
    const bool allowAllUrlSchemes = qzSettings->allowAllUrlSchemes; // https://doc.qt.io/qt-5/qwebenginesettings.html#UnknownUrlSchemePolicy-enum

    if (userInteractUrlSchemes) {
        webSettings->setUnknownUrlSchemePolicy(allowAllUrlSchemes ? QWebEngineSettings::AllowUnknownUrlSchemesFromUserInteraction : QWebEngineSettings::DisallowUnknownUrlSchemes);
    } else {
        webSettings->setUnknownUrlSchemePolicy(allowAllUrlSchemes ? QWebEngineSettings::AllowAllUnknownUrlSchemes : QWebEngineSettings::DisallowUnknownUrlSchemes);
    }

    QWebEngineProfile* profile = QWebEngineProfile::defaultProfile();

    profile->setPersistentStoragePath(DataPaths::currentProfilePath());

    QString defaultPath = DataPaths::path(DataPaths::Cache);
    if (!defaultPath.startsWith(DataPaths::currentProfilePath()))
        defaultPath.append(QLatin1Char('/') + ProfileManager::currentProfile());
    const QString &cachePath = settings.value(QStringLiteral("Web-Browser-Settings/CachePath"), defaultPath).toString();
    settings.setValue(QStringLiteral("Web-Browser-Settings/CachePath"), cachePath);
    profile->setCachePath(cachePath);

    const bool allowCache = qzSettings->allowLocalCache;
    profile->setHttpCacheType(allowCache ? QWebEngineProfile::DiskHttpCache : QWebEngineProfile::MemoryHttpCache);

    const int cacheSize = qzSettings->localCacheSize * 1000 * 1000;
    profile->setHttpCacheMaximumSize(cacheSize);

    profile->setSpellCheckEnabled(qzSettings->enabled);
    profile->setSpellCheckLanguages(qzSettings->languages);

    const bool allowPersistentCookies = qzSettings->allowPersistentCookies; // https://doc.qt.io/qt-5/qwebengineprofile.html#PersistentCookiesPolicy-enum
    profile->setPersistentCookiesPolicy(allowPersistentCookies ? QWebEngineProfile::AllowPersistentCookies : QWebEngineProfile::NoPersistentCookies);

    loadTheme(qzSettings->activeTheme);
    setUserStyleSheet(qzSettings->userStyleSheet);

}

void MainApplication::loadTheme(const QString &name) {

    QString activeThemePath = DataPaths::locate(DataPaths::Themes, name);
    QString themeName = name;
    QString qss;
    Settings settings;

    qDebug() << "Looking for themes in " << DataPaths::path(DataPaths::Themes);

    if (activeThemePath.isEmpty()) {

        if (themeName != QLatin1String("None")) {

#if !defined(Q_OS_WIN)
            qDebug() << "Looking for themes in " << DataPaths::path(DataPaths::ThemesExtra);
            activeThemePath = DataPaths::locate(DataPaths::ThemesExtra, name);
#endif

            if (activeThemePath.isEmpty()) {
                qWarning() << "Cannot load theme " << name;
                themeName = QLatin1String("None");
            }
        }
    }

    BrowserWindow *window = mApp->getWindow();

    if (themeName != QLatin1String("None")) {

        qss = QzTools::readAllFileContents(activeThemePath + QLatin1String("/main.css"));

        if (themeName == "Vivid") {
            QString commonThemePath = DataPaths::locate(DataPaths::Themes, QString("Noir"));
#if !defined(Q_OS_WIN)
            if (commonThemePath.isEmpty()) {
                commonThemePath = DataPaths::locate(DataPaths::ThemesExtra, QString("Noir"));
            }
#endif
            qss.append(QzTools::readAllFileContents(commonThemePath + QLatin1String("/common.css")));
        } else {
            qss.append(QzTools::readAllFileContents(activeThemePath + QLatin1String("/common.css")));
        }

        qss.append(QzTools::readAllFileContents(activeThemePath + QLatin1String("/") + themeName + QLatin1String(".css")));

        if (isRightToLeft()) {
            qss.append(QzTools::readAllFileContents(activeThemePath + QLatin1String("/rtl.css")));
        }

        if (isPrivate()) {
            qss.append(QzTools::readAllFileContents(activeThemePath + QLatin1String("/private.css")));
        }

        qss.append(QzTools::readAllFileContents(DataPaths::currentProfilePath() + QLatin1String("/userChrome.css")));

        QString relativePath = QDir::current().relativeFilePath(activeThemePath);
        qss.replace(QRegularExpression(QStringLiteral("url\\s*\\(\\s*([^\\*:\\);]+)\\s*\\)")), QStringLiteral("url(%1/\\1)").arg(relativePath));

        window->navigationBar()->m_buttonBack->setIcon(QIcon());
        window->navigationBar()->m_buttonForward->setIcon(QIcon());
        window->navigationBar()->m_buttonHome->setIcon(QIcon());
        window->navigationBar()->m_supMenu->setIcon(QIcon());
        window->navigationBar()->m_exitFullscreen->setIcon(QIcon());
        window->navigationBar()->m_buttonAddTab->setIcon(QIcon());
        window->tabWidget()->m_buttonAddTab->setIcon(QIcon());
        window->tabWidget()->m_buttonAddTab2->setIcon(QIcon());
        window->navigationBar()->m_buttonTools->setIcon(QIcon());
        window->navigationBar()->m_reloadStop->setIcon(QIcon());

    } else {

        window->navigationBar()->m_buttonBack->setIcon(QIcon(QStringLiteral(":/icons/menu/go-previous.svg")));
        window->navigationBar()->m_buttonForward->setIcon(QIcon(QStringLiteral(":/icons/menu/go-next.svg")));
        window->navigationBar()->m_buttonHome->setIcon(QIcon(QStringLiteral(":/icons/menu/go-home.svg")));
        window->navigationBar()->m_supMenu->setIcon(QIcon(QStringLiteral(":/icons/menu/sub-menu.svg")));
        window->navigationBar()->m_exitFullscreen->setIcon(QIcon(QStringLiteral(":/icons/menu/view-fullscreen.svg")));
        window->navigationBar()->m_buttonAddTab->setIcon(QIcon(QStringLiteral(":/icons/menu/tab-new.svg")));
        window->tabWidget()->m_buttonAddTab->setIcon(QIcon(QStringLiteral(":/icons/menu/tab-new.svg")));
        window->tabWidget()->m_buttonAddTab2->setIcon(QIcon(QStringLiteral(":/icons/menu/tab-new.svg")));
        window->navigationBar()->m_buttonTools->setIcon(QIcon(QStringLiteral(":/icons/menu/menu.svg")));
        window->navigationBar()->m_reloadStop->setIcon(QIcon(QStringLiteral(":/icons/menu/view-refresh.svg")));

        settings.beginGroup(QStringLiteral("Themes"));
        settings.setValue(QStringLiteral("activeTheme"), QStringLiteral("None"));
        settings.endGroup();

    }

    settings.beginGroup(QStringLiteral("Browser-Fonts"));
    const QString defFont = settings.value(QStringLiteral("DefaultFontSize")).toString();
    settings.endGroup();

    if (defFont.isEmpty()) {

        qss.append(QLatin1String("QWidget{font-size:14px !important}")); // Fallback if the setting doesn't exist

    } else {

        qss.append(QLatin1String("QWidget{font-size:") + defFont + QLatin1String("px !important}")); // Important default font variable

    }

    setStyleSheet(qss);

}

void MainApplication::checkOptimizeDatabase()
{
    Settings settings;
    settings.beginGroup(QStringLiteral("Browser"));
    const int numberOfRuns = settings.value(QStringLiteral("RunsWithoutOptimizeDb"), 0).toInt();
    settings.setValue(QStringLiteral("RunsWithoutOptimizeDb"), numberOfRuns + 1);

    if (numberOfRuns > 20) {
        std::cout << "Optimizing database..." << std::endl;
        IconProvider::instance()->clearOldIconsInDatabase();
        settings.setValue(QStringLiteral("RunsWithoutOptimizeDb"), 0);
    }

    settings.endGroup();
}

void MainApplication::setupUserScripts()
{
    // WebChannel for jsAppWorldSafe
    QWebEngineScript script;
    script.setName(QStringLiteral("_browser_webchannel"));
    script.setInjectionPoint(QWebEngineScript::DocumentCreation);
    script.setWorldId(WebPage::jsAppWorldSafe);
    script.setRunsOnSubFrames(true);
    script.setSourceCode(Scripts::setupWebChannel());
    m_webProfile->scripts()->insert(script);
    // browser:restore
    QWebEngineScript bhawkRestore;
    bhawkRestore.setWorldId(WebPage::jsAppWorldSafe);
    bhawkRestore.setSourceCode(QzTools::readAllFileContents(QStringLiteral(":html/restore.user.js")));
    m_webProfile->scripts()->insert(bhawkRestore);
    // browser:speeddial
    QWebEngineScript bhawkSpeedDial;
    bhawkSpeedDial.setWorldId(WebPage::jsAppWorldSafe);
    bhawkSpeedDial.setSourceCode(Scripts::setupSpeedDial());
    m_webProfile->scripts()->insert(bhawkSpeedDial);

    if (qzSettings->httpsByDefault) {
        // re-write http links to https
        QWebEngineScript documentWindowAddons;
        documentWindowAddons.setName(QStringLiteral("_browser_window_object"));
        documentWindowAddons.setInjectionPoint(QWebEngineScript::DocumentCreation);
        documentWindowAddons.setWorldId(WebPage::jsAppWorldSafe);
        documentWindowAddons.setRunsOnSubFrames(true);
        documentWindowAddons.setSourceCode(Scripts::universalJs());
        m_webProfile->scripts()->insert(documentWindowAddons);

    }

}

void MainApplication::setUserStyleSheet(const QString &filePath)
{
    QString userCss;

    QString highlightColor = qzSettings->pageHighlightColor;
    QString highlightedTextColor = qzSettings->pageHighlightedTextColor;

    userCss += QString("::selection{background: %1; color: %2} ").arg(highlightColor, highlightedTextColor);
    userCss += QzTools::readAllFileContents(filePath).remove(QLatin1Char('\n'));
    const QString name = QStringLiteral("_browser_userstylesheet");
    QWebEngineScript oldScript = m_webProfile->scripts()->findScript(name);

    if (!oldScript.isNull()) {
        m_webProfile->scripts()->remove(oldScript);
    }

    if (userCss.isEmpty())
        return;

    QWebEngineScript script;
    script.setName(name);
    script.setInjectionPoint(QWebEngineScript::DocumentReady);
    script.setWorldId(WebPage::jsAppWorldSafe);
    script.setRunsOnSubFrames(true);
    script.setSourceCode(Scripts::setCss(userCss));
    m_webProfile->scripts()->insert(script);

}

void MainApplication::createJumpList()
{
#ifdef Q_OS_WIN
    QWinJumpList *jumpList = new QWinJumpList(this);
    jumpList->clear();

    // Frequent
    QWinJumpListCategory *frequent = jumpList->frequent();
    frequent->setVisible(true);
    const QVector<HistoryEntry> mostList = m_history->mostVisited(7);
    for (const HistoryEntry &entry : mostList) {
        frequent->addLink(IconProvider::iconForUrl(entry.url), entry.title, applicationFilePath(), QStringList{entry.url.toEncoded()});
    }

    // Tasks
    QWinJumpListCategory *tasks = jumpList->tasks();
    tasks->setVisible(true);
    tasks->addLink(IconProvider::newTabIcon(), tr("Open new tab"), applicationFilePath(), {QStringLiteral("--opt-u")});
    tasks->addLink(IconProvider::newWindowIcon(), tr("Open new window"), applicationFilePath(), {QStringLiteral("--opt-i")});
    tasks->addLink(IconProvider::privateBrowsingIcon(), tr("Open new private window"), applicationFilePath(), {QStringLiteral("--opt-c")});
#endif
}

#ifdef Q_OS_LINUX
void MainApplication::initPulseSupport()
{
    const QByteArray appName = QString("midori").toLatin1();

    qputenv("PULSE_PROP_OVERRIDE_application.name", appName);
    qputenv("PULSE_PROP_OVERRIDE_application.icon_name", appName);
    qputenv("PULSE_PROP_OVERRIDE_media.icon_name", appName);
}
#endif

#if defined(Q_OS_WIN) && !defined(Q_OS_OS2)
RegisterQAppAssociation* MainApplication::associationManager()
{
    if (!m_registerQAppAssociation) {
        QString desc = tr("Midori Browser is licensed under GPL version 3 or (at your option) any later version. It is uses the Qt Framework and it's QtWebEngine component.");
        QString fileIconPath = QApplication::applicationFilePath() + ",1";
        QString appIconPath = QApplication::applicationFilePath() + ",0";
        m_registerQAppAssociation = new RegisterQAppAssociation("Midori Browser", QApplication::applicationFilePath(), appIconPath, desc, this);
        m_registerQAppAssociation->addCapability(".html", "BhawkHTML", "Midori HTML Document", fileIconPath, RegisterQAppAssociation::FileAssociation);
        m_registerQAppAssociation->addCapability(".htm", "BhawkHTML", "Midori HTML Document", fileIconPath, RegisterQAppAssociation::FileAssociation);
        m_registerQAppAssociation->addCapability(".asp", "BhawkHTML", "Midori HTML Document", fileIconPath, RegisterQAppAssociation::FileAssociation);
        m_registerQAppAssociation->addCapability(".aspx", "BhawkHTML", "Midori HTML Document", fileIconPath, RegisterQAppAssociation::FileAssociation);
        m_registerQAppAssociation->addCapability(".jsp", "BhawkHTML", " Midori HTML Document", fileIconPath, RegisterQAppAssociation::FileAssociation);
        m_registerQAppAssociation->addCapability(".mhtml", "BhawkHTML", "Midori HTML Document", fileIconPath, RegisterQAppAssociation::FileAssociation);
        m_registerQAppAssociation->addCapability(".mht", "BhawkHTML", "Midori HTML Document", fileIconPath, RegisterQAppAssociation::FileAssociation);
        m_registerQAppAssociation->addCapability(".php", "BhawkHTML", "Midori HTML Document", fileIconPath, RegisterQAppAssociation::FileAssociation);
        m_registerQAppAssociation->addCapability(".phtm", "BhawkHTML", "Midori HTML Document", fileIconPath, RegisterQAppAssociation::FileAssociation);
        m_registerQAppAssociation->addCapability(".phtml", "BhawkHTML", "Midori HTML Document", fileIconPath, RegisterQAppAssociation::FileAssociation);
        m_registerQAppAssociation->addCapability(".xht", "BhawkHTML", "Midori HTML Document", fileIconPath, RegisterQAppAssociation::FileAssociation);
        m_registerQAppAssociation->addCapability(".xhtm", "BhawkHTML", "Midori HTML Document", fileIconPath, RegisterQAppAssociation::FileAssociation);
        m_registerQAppAssociation->addCapability(".xhtml", "BhawkHTML", "Midori HTML Document", fileIconPath, RegisterQAppAssociation::FileAssociation);
        m_registerQAppAssociation->addCapability("http", "BhawkURL", "Midori URL", appIconPath, RegisterQAppAssociation::UrlAssociation);
        m_registerQAppAssociation->addCapability("https", "BhawkURL", "Midori URL", appIconPath, RegisterQAppAssociation::UrlAssociation);
        m_registerQAppAssociation->addCapability("ftp", "BhawkURL", "Midori URL", appIconPath, RegisterQAppAssociation::UrlAssociation);
        m_registerQAppAssociation->addCapability("ftps", "BhawkURL", "Midori URL", appIconPath, RegisterQAppAssociation::UrlAssociation);
    }
    return m_registerQAppAssociation;
}
#endif

#ifdef Q_OS_MACOS
#include <QFileOpenEvent>

bool MainApplication::event(QEvent* e)
{
    switch (e->type()) {
    case QEvent::FileOpen: {
        QFileOpenEvent *ev = static_cast<QFileOpenEvent*>(e);
        if (!ev->url().isEmpty()) {
            addNewTab(ev->url());
        } else if (!ev->file().isEmpty()) {
            addNewTab(QUrl::fromLocalFile(ev->file()));
        } else {
            return false;
        }
        return true;
    }

    case QEvent::ApplicationActivate:
        if (!activeWindow() && m_windows.isEmpty())
            createWindow(Qz::BW_NewWindow);
        break;

    default:
        break;
    }

    return QtSingleApplication::event(e);
}
#endif
