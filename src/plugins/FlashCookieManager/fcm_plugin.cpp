/* ============================================================
* FlashCookieManager plugin for Falkon
* Copyright (C) 2014-2018 S. Razi Alavizadeh <s.r.alavizadeh@gmail.com>
* Copyright (C) 2017-2018 David Rosca <nowrep@gmail.com>
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
#include "fcm_plugin.h"
#include "browserwindow.h"
#include "pluginproxy.h"
#include "mainapplication.h"
#include "fcm_dialog.h"
#include "abstractbuttoninterface.h"
#include "tabbedwebview.h"
#include "fcm_notification.h"
#include "datapaths.h"
#include "../config.h"
#include "statusbar.h"
#include "navigationbar.h"

#include <QTimer>
#include <QSettings>
#include <QDir>
#include <QMenu>

#if defined(Q_OS_WIN) || defined(Q_OS_OS2)
#include <QProcessEnvironment>
#endif

const int refreshInterval = 60 * 1000;

class FCM_Button : public AbstractButtonInterface
{
    Q_OBJECT
public:
    explicit FCM_Button(QObject *parent = nullptr)
        : AbstractButtonInterface(parent)
    {
    }

    QString id() const override
    {
        return QStringLiteral("fcm-icon");
    }

    QString name() const override
    {
        return tr("Flash Cookie Manager button");
    }
};

FCM_Plugin::FCM_Plugin()
    : QObject()
{
}

void FCM_Plugin::init(InitState state, const QString &settingsPath)
{
    m_settingsPath = settingsPath;

    connect(mApp->plugins(), &PluginProxy::mainWindowCreated, this, &FCM_Plugin::mainWindowCreated);
    connect(mApp->plugins(), &PluginProxy::mainWindowDeleted, this, &FCM_Plugin::mainWindowDeleted);

    m_timer = new QTimer(this);
    m_timer->setInterval(refreshInterval);
    connect(m_timer, &QTimer::timeout, this, &FCM_Plugin::autoRefresh);

    // start timer if needed
    startStopTimer();

    if (state == StartupInitState && readSettings().value(QLatin1String("deleteAllOnStartExit")).toBool()) {
        loadFlashCookies();

        removeAllButWhitelisted();
    }

    if (state == LateInitState) {
        const auto windows = mApp->windows();
        for (BrowserWindow* window : windows) {
            mainWindowCreated(window);
        }
    }
}

void FCM_Plugin::unload()
{
    if (m_fcmDialog) {
        m_fcmDialog->close();
    }

    if (mApp->isClosing() && readSettings().value(QLatin1String("deleteAllOnStartExit")).toBool()) {
        removeAllButWhitelisted();
    }

    const auto windows = mApp->windows();
    for (BrowserWindow* window : windows) {
        mainWindowDeleted(window);
    }

    delete m_fcmDialog;
}

bool FCM_Plugin::testPlugin()
{
    return (Qz::VERSION == QLatin1String(MIDORI_VERSION));
}

void FCM_Plugin::showSettings(QWidget* parent)
{
    Q_UNUSED(parent)

    showFlashCookieManager();
    m_fcmDialog->showPage(2);
}

void FCM_Plugin::populateExtensionsMenu(QMenu* menu)
{
    QAction* showFCM = new QAction(QIcon(":/flashcookiemanager/data/flash-cookie-manager.png"), tr("Flash Cookie Manager"), menu);
    connect(showFCM, &QAction::triggered, this, &FCM_Plugin::showFlashCookieManager);
    menu->addAction(showFCM);
}

void FCM_Plugin::setFlashCookies(const QList<FlashCookie> &flashCookies)
{
    m_flashCookies = flashCookies;
}

QList<FlashCookie> FCM_Plugin::flashCookies()
{
    if (m_flashCookies.isEmpty()) {
        loadFlashCookies();
    }
    return m_flashCookies;
}

QStringList FCM_Plugin::newCookiesList()
{
    return m_newCookiesList;
}

void FCM_Plugin::clearNewOrigins()
{
    m_newCookiesList.clear();
}

void FCM_Plugin::clearCache()
{
    m_flashCookies.clear();
}

bool FCM_Plugin::isBlacklisted(const FlashCookie &flashCookie)
{
    return readSettings().value(QLatin1String("flashCookiesBlacklist")).toStringList().contains(flashCookie.origin);
}

bool FCM_Plugin::isWhitelisted(const FlashCookie &flashCookie)
{
    return readSettings().value(QLatin1String("flashCookiesWhitelist")).toStringList().contains(flashCookie.origin);
}

void FCM_Plugin::removeAllButWhitelisted()
{
    for (const FlashCookie &flashCookie : qAsConst(m_flashCookies)) {
        if (isWhitelisted(flashCookie)) {
            continue;
        }

        removeCookie(flashCookie);
    }
}

QString FCM_Plugin::sharedObjectDirName() const
{
    if (flashPlayerDataPath().contains(QLatin1String("macromedia"), Qt::CaseInsensitive) ||
            !flashPlayerDataPath().contains(QLatin1String("/.gnash"), Qt::CaseInsensitive)) {
        return QLatin1String(QLatin1String("/#SharedObjects/"));
    }
    else {
        return QLatin1String(QLatin1String("/SharedObjects/"));
    }
}

QString FCM_Plugin::flashPlayerDataPath() const
{
    return DataPaths::currentProfilePath() + QStringLiteral("/Pepper Data/Shockwave Flash/WritableRoot/");
}

QVariantHash FCM_Plugin::readSettings() const
{
    if (m_settingsHash.isEmpty()) {
        m_settingsHash.insert(QLatin1String("autoMode"), QVariant(false));
        m_settingsHash.insert(QLatin1String("deleteAllOnStartExit"), QVariant(false));
        m_settingsHash.insert(QLatin1String("notification"), QVariant(false));
        m_settingsHash.insert(QLatin1String("flashCookiesWhitelist"), QVariant());
        m_settingsHash.insert(QLatin1String("flashCookiesBlacklist"), QVariant());

        QSettings settings(m_settingsPath + QLatin1String("/plug-ins.ini"), QSettings::IniFormat);
        settings.beginGroup(QLatin1String("FlashCookieManager"));
        QVariantHash::iterator i = m_settingsHash.begin();
        while (i != m_settingsHash.end()) {
            *i = settings.value(i.key(), i.value());
            ++i;
        }

        settings.endGroup();
    }

    return m_settingsHash;
}

void FCM_Plugin::writeSettings(const QVariantHash &hashSettings)
{
    m_settingsHash = hashSettings;

    QSettings settings(m_settingsPath + QLatin1String(QLatin1String("/plug-ins.ini")), QSettings::IniFormat);
    settings.beginGroup(QLatin1String("FlashCookieManager"));
    QVariantHash::const_iterator i = m_settingsHash.constBegin();
    while (i != m_settingsHash.constEnd()) {
        settings.setValue(i.key(), i.value());
        ++i;
    }

    settings.endGroup();

    startStopTimer();
}

void FCM_Plugin::removeCookie(const FlashCookie &flashCookie)
{
    if (m_flashCookies.contains(flashCookie)) {
        m_flashCookies.removeOne(flashCookie);
        if (QFile(flashCookie.path + QLatin1Char('/') + flashCookie.name).remove()) {
            QDir dir(flashCookie.path);
            dir.rmpath(flashCookie.path);
        }
    }
}

void FCM_Plugin::autoRefresh()
{
    if (m_fcmDialog && m_fcmDialog->isVisible()) {
        return;
    }

    QList<FlashCookie> oldflashCookies = m_flashCookies;
    loadFlashCookies();
    QStringList newCookieList;

    for (const FlashCookie &flashCookie : qAsConst(m_flashCookies)) {
        if (isBlacklisted(flashCookie)) {
            removeCookie(flashCookie);
            continue;
        }

        if (isWhitelisted(flashCookie)) {
            continue;
        }

        bool newCookie = true;
        for (const FlashCookie &oldFlashCookie : qAsConst(oldflashCookies)) {
            if (QString(oldFlashCookie.path + oldFlashCookie.name) ==
                    QString(flashCookie.path + flashCookie.name)) {
                newCookie = false;
                break;
            }
        }

        if (newCookie) {
            newCookieList << flashCookie.path + QLatin1Char('/') + flashCookie.name;
        }
    }

    if (!newCookieList.isEmpty() && readSettings().value(QLatin1String("notification")).toBool()) {
        m_newCookiesList << newCookieList;
        BrowserWindow* window = mApp->getWindow();
        if (!window) {
            return;
        }
        TabbedWebView* weView = window->weView();
        if (!weView) {
            return;
        }

        FCM_Notification* notif = new FCM_Notification(this, newCookieList.size());
        weView->addNotification(notif);
    }
}

void FCM_Plugin::showFlashCookieManager()
{
    if (!m_fcmDialog) {
        m_fcmDialog = new FCM_Dialog(this);
    }

    m_fcmDialog->refreshView();
    m_fcmDialog->showPage(0);
    m_fcmDialog->show();
    m_fcmDialog->raise();
}

void FCM_Plugin::mainWindowCreated(BrowserWindow *window)
{
    window->statusBar()->addButton(createStatusBarIcon(window));
    window->navigationBar()->addToolButton(createStatusBarIcon(window));
}

void FCM_Plugin::mainWindowDeleted(BrowserWindow *window)
{
    if (!window) {
        return;
    }

    if (m_fcmDialog && m_fcmDialog->parent() == window) {
        m_fcmDialog->setParent(0);
    }

    window->statusBar()->removeButton(m_statusBarIcons.value(window));
    window->navigationBar()->removeToolButton(m_statusBarIcons.value(window));

    delete m_statusBarIcons.value(window);
    m_statusBarIcons.remove(window);
}

void FCM_Plugin::startStopTimer()
{
    if (readSettings().value(QLatin1String("autoMode")).toBool()) {
        if (!m_timer->isActive()) {
            if (m_flashCookies.isEmpty()) {
                loadFlashCookies();
            }

            m_timer->start();
        }
    }
    else {
        m_timer->stop();
    }
}

AbstractButtonInterface* FCM_Plugin::createStatusBarIcon(BrowserWindow* mainWindow)
{
    if (m_statusBarIcons.contains(mainWindow)) {
        return m_statusBarIcons.value(mainWindow);
    }

    FCM_Button* icon = new FCM_Button(this);
    icon->setIcon(QIcon(QStringLiteral(":/flashcookiemanager/data/flash-cookie-manager.png")));
    icon->setTitle(tr("Flash Cookie Manager"));
    icon->setToolTip(tr("Show Flash Cookie Manager"));
    connect(icon, &AbstractButtonInterface::clicked, this, &FCM_Plugin::showFlashCookieManager);

    m_statusBarIcons.insert(mainWindow, icon);

    return icon;
}

void FCM_Plugin::loadFlashCookies()
{
    m_flashCookies.clear();
    loadFlashCookies(flashPlayerDataPath());
}

void FCM_Plugin::loadFlashCookies(QString path)
{
    QDir solDir(path);
    QStringList entryList = solDir.entryList();
    entryList.removeAll(QLatin1String("."));
    entryList.removeAll(QLatin1String(".."));

    for (QString entry : qAsConst(entryList)) {
        if (path.endsWith(QLatin1String("#SharedObjects")) && entry == QLatin1String("#AppContainer")) {
            // specific to IE and Windows
            continue;
        }

        path.replace(QLatin1Char('\\'), QLatin1Char('/'));
        QFileInfo entryInfo(path + QLatin1Char('/') + entry);
        if (entryInfo.isDir()) {
            loadFlashCookies(entryInfo.filePath());
        }
        else if (entryInfo.isFile() && entryInfo.suffix() == QLatin1String("sol")) {
            insertFlashCookie(entryInfo.filePath());
        }
    }
}

void FCM_Plugin::insertFlashCookie(const QString &path)
{
    QFile solFile(path);
    if (!solFile.open(QFile::ReadOnly)) {
        return;
    }

    QByteArray file = solFile.readAll();
    for (int i = 0; i < file.size(); ++i) {
        if (!((file.at(i) >= 'a' && file.at(i) <= 'z') || (file.at(i) >= 'A' && file.at(i) <= 'Z') ||
              (file.at(i) >= '0' && file.at(i) <= '9'))) {
            file[i] = ' ';
        }
    }

    QString fileStr = QString(file);
    fileStr = fileStr.split(QLatin1Char('.'), QString::SkipEmptyParts).join(QLatin1String("\n"));

    QFileInfo solFileInfo(solFile);

    FlashCookie flashCookie;
    flashCookie.contents = fileStr;
    flashCookie.name = solFileInfo.fileName();
    flashCookie.path = solFileInfo.canonicalPath();
    flashCookie.size = (int)solFile.size();
    flashCookie.lastModification = solFileInfo.lastModified();
    flashCookie.origin = extractOriginFrom(path);

    m_flashCookies << flashCookie;
}

QString FCM_Plugin::extractOriginFrom(const QString &path)
{
    QString origin = path;
    if (path.startsWith(flashPlayerDataPath() + sharedObjectDirName())) {
        origin.remove(flashPlayerDataPath() + sharedObjectDirName());
        if (origin.indexOf(QLatin1Char('/')) != -1) {
            origin.remove(0, origin.indexOf(QLatin1Char('/')) + 1);
        }
    }
    else if (path.startsWith(flashPlayerDataPath() + QLatin1String("/macromedia.com/support/flashplayer/sys/"))) {
        origin.remove(flashPlayerDataPath() + QLatin1String("/macromedia.com/support/flashplayer/sys/"));
        if (origin == QLatin1String("settings.sol")) {
            return tr("!default");
        }
        else if (origin.startsWith(QLatin1Char('#'))) {
            origin.remove(0, 1);
        }
    }
    else {
        origin.clear();
    }

    int index = origin.indexOf(QLatin1Char('/'));
    if (index == -1) {
        return tr("!other");
    }
    origin.remove(index, origin.size());
    if (origin == QLatin1String("localhost") || origin == QLatin1String("local")) {
        origin = QLatin1String("!localhost");
    }

    return origin;
}

#include "fcm_plugin.moc"
