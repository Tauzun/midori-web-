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
#include "preferences.h"
#include "ui_preferences.h"
#include "browserwindow.h"
#include "bookmarkstoolbar.h"
#include "history.h"
#include "tabwidget.h"
#include "cookiejar.h"
#include "locationbar.h"
#include "autofillmanager.h"
#include "mainapplication.h"
#include "cookiemanager.h"
#include "pluginproxy.h"
#include "pluginsmanager.h"
#include "jsoptions.h"
#include "networkmanager.h"
#include "desktopnotificationsfactory.h"
#include "desktopnotification.h"
#include "thememanager.h"
#include "acceptlanguage.h"
#include "qztools.h"
#include "autofill.h"
#include "settings.h"
#include "datapaths.h"
#include "tabbedwebview.h"
#include "clearprivatedata.h"
#include "useragentdialog.h"
#include "registerqappassociation.h"
#include "profilemanager.h"
#include "html5permissions/html5permissionsdialog.h"
#include "searchenginesdialog.h"
#include "webscrollbarmanager.h"
#include "protocolhandlerdialog.h"
#include "../config.h"
#include "qzsettings.h"

#include <QSettings>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QColorDialog>
#include <QDesktopWidget>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QLibraryInfo>
#include <QtWebEngineWidgetsVersion>
#include <QStandardPaths>

static QString createLanguageItem(const QString &lang)
{
    QLocale locale(lang);

    if (locale.language() == QLocale::C) {
        return lang;
    }

    QString country = QLocale::countryToString(locale.country());
    QString language = QLocale::languageToString(locale.language());

    if (lang == QLatin1String("es_ES")) {
        return QString::fromUtf8("Castellano");
    }
    if (lang == QLatin1String("nqo")) {
        return QString("N'ko (nqo)");
    }
    if (lang == QLatin1String("sr")) {
        return QString::fromUtf8("српски екавски");
    }
    if (lang == QLatin1String("sr@ijekavian")) {
        return QString::fromUtf8("српски ијекавски");
    }
    if (lang == QLatin1String("sr@latin")) {
        return QString::fromUtf8("srpski ekavski");
    }
    if (lang == QLatin1String("sr@ijekavianlatin")) {
        return QString::fromUtf8("srpski ijekavski");
    }
    return QString("%1, %2 (%3)").arg(language, country, lang);
}

Preferences::Preferences(BrowserWindow* window)
    : QDialog(window)
    , ui(new Ui::Preferences)
    , m_window(window)
    , m_autoFillManager(0)
    , m_pluginsList(0)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    QzTools::centerWidgetOnScreen(this);

    m_themesManager = new ThemeManager(ui->themesWidget, this);
    m_pluginsList = new PluginsManager(this);
    ui->pluginsFrame->addWidget(m_pluginsList);

#ifdef DISABLE_CHECK_UPDATES
    ui->checkUpdates->setVisible(false);
#endif

    std::function<void (int, const QIcon &)> setCategoryIcon = [this](int index, const QIcon &icon) {
        ui->listWidget->item(index)->setIcon(QIcon(icon.pixmap(32)));
    };

    setCategoryIcon(0, QIcon(":/icons/preferences/general.svg"));
    setCategoryIcon(1, QIcon(":/icons/preferences/appearance.svg"));
    setCategoryIcon(2, QIcon(":/icons/preferences/tabs.svg"));
    setCategoryIcon(3, QIcon(":/icons/preferences/browsing.svg"));
    setCategoryIcon(4, QIcon(":/icons/preferences/fonts.svg"));
    setCategoryIcon(5, QIcon(":/icons/preferences/shortcuts.svg"));
    setCategoryIcon(6, QIcon(":/icons/preferences/downloads.svg"));
    setCategoryIcon(7, QIcon(":/icons/preferences/passwords.svg"));
    setCategoryIcon(8, QIcon(":/icons/preferences/privacy.svg"));
    setCategoryIcon(9, QIcon(":/icons/preferences/notifications.svg"));
    setCategoryIcon(10, QIcon(":/icons/preferences/extensions.svg"));
    setCategoryIcon(11, QIcon(":/icons/preferences/spellcheck.svg"));
    setCategoryIcon(12, QIcon(":/icons/preferences/other.svg"));

    mApp->reloadSettings();
    //GENERAL URLs
    m_homepage = qzSettings->homepage;
    m_newTabUrl = qzSettings->newTabUrl;

    ui->homepage->setText(m_homepage.toEncoded());
    ui->newTabUrl->setText(m_newTabUrl.toEncoded());
    ui->afterLaunch->setCurrentIndex(qzSettings->afterlaunch);

    Settings settings;

#ifdef DISABLE_CHECK_UPDATES
    ui->checkUpdates->setChecked(settings.value("Web-Browser-Settings/CheckUpdates", false).toBool());
#else
    ui->checkUpdates->setChecked(settings.value("Web-Browser-Settings/CheckUpdates", true).toBool());
#endif

    ui->dontLoadTabsUntilSelected->setChecked(qzSettings->loadTabsOnActivation);

#if defined(Q_OS_WIN) && !defined(Q_OS_OS2)
    ui->checkDefaultBrowser->hide();
    ui->checkNowDefaultBrowser->hide();
#else // No Default Browser settings on non-Windows platform / This is difficult to handle in gnome3 and kde5
    ui->hSpacerDefaultBrowser->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->hLayoutDefaultBrowser->invalidate();
    delete ui->hLayoutDefaultBrowser;
    delete ui->checkDefaultBrowser;
    delete ui->checkNowDefaultBrowser;
#endif

    ui->newTabFrame->setVisible(false);

    if (m_newTabUrl.isEmpty() || m_newTabUrl.toString() == QLatin1String("about:blank")) {
        ui->newTab->setCurrentIndex(0);
    }
    else if (m_newTabUrl == m_homepage) {
        ui->newTab->setCurrentIndex(1);
    }
    else if (m_newTabUrl.toString() == QLatin1String("browser:speeddial")) {
        ui->newTab->setCurrentIndex(2);
    }
    else {
        ui->newTab->setCurrentIndex(3);
        ui->newTabFrame->setVisible(true);
    }

    afterLaunchChanged(ui->afterLaunch->currentIndex());
    connect(ui->afterLaunch, SIGNAL(currentIndexChanged(int)), this, SLOT(afterLaunchChanged(int)));
    connect(ui->newTab, SIGNAL(currentIndexChanged(int)), this, SLOT(newTabChanged(int)));
    if (m_window) {
        connect(ui->useCurrentBut, &QAbstractButton::clicked, this, &Preferences::useActualHomepage);
        connect(ui->newTabUseCurrent, &QAbstractButton::clicked, this, &Preferences::useActualNewTab);
    }
    else {
        ui->useCurrentBut->setEnabled(false);
        ui->newTabUseCurrent->setEnabled(false);
    }
    // PROFILES
    QString startingProfile = ProfileManager::startingProfile();
    ui->activeProfile->setText("<b>" + ProfileManager::currentProfile() + "</b>");
    ui->startProfile->addItem(startingProfile);

    const QStringList names = ProfileManager::availableProfiles();

    for (const QString &name : names) {
        if (startingProfile != name) {
            ui->startProfile->addItem(name);
        }
    }

    connect(ui->createProfile, &QAbstractButton::clicked, this, &Preferences::createProfile);
    connect(ui->deleteProfile, &QAbstractButton::clicked, this, &Preferences::deleteProfile);
    connect(ui->startProfile, SIGNAL(currentIndexChanged(int)), this, SLOT(startProfileIndexChanged(int)));
    startProfileIndexChanged(ui->startProfile->currentIndex());
    //APPEARANCE
    ui->showStatusbar->setChecked(qzSettings->showStatusBar);
    ui->showSearchbar->setChecked(qzSettings->showSearchBar);
    ui->instantBookmarksToolbar->setChecked(qzSettings->instantBookmarksToolbar);
    ui->showBookmarksToolbar->setChecked(qzSettings->showBookmarksToolbar);
    ui->instantBookmarksToolbar->setDisabled(qzSettings->showBookmarksToolbar);
    ui->showBookmarksToolbar->setDisabled(qzSettings->instantBookmarksToolbar);
    ui->showNavigationToolbar->setChecked(qzSettings->showNavigationToolbar);
    ui->navInFullScreen->setChecked(qzSettings->showNavInFullScreen);
    ui->locBarAutoHide->setChecked(qzSettings->fullScreenLocationBarAutoHide);
    int currentSettingsPage = qzSettings->settingsDialogPage;

    connect(ui->instantBookmarksToolbar, &QAbstractButton::toggled, ui->showBookmarksToolbar, &QWidget::setDisabled);
    connect(ui->showBookmarksToolbar, &QAbstractButton::toggled, ui->instantBookmarksToolbar, &QWidget::setDisabled);
    connect(ui->showSearchbar, &QAbstractButton::toggled, window->navigationBar()->webSearchBar(), &QWidget::setVisible);
    //TABS
    ui->hideTabsOnTab->setChecked(qzSettings->hideTabsWithOneTab); // Change in tabbar.cpp for this to activate at startup
    ui->activateLastTab->setChecked(qzSettings->activateLastTabWhenClosingActual);
    ui->openNewTabAfterActive->setChecked(qzSettings->newTabAfterActive);
    ui->openNewEmptyTabAfterActive->setChecked(qzSettings->newEmptyTabAfterActive);
    ui->openPopupsInTabs->setChecked(qzSettings->openPopupsInTabs);
    ui->alwaysSwitchTabsWithWheel->setChecked(qzSettings->alwaysSwitchTabsWithWheel);
    ui->switchToNewTabs->setChecked(qzSettings->newTabPos);
    ui->dontCloseOnLastTab->setChecked(qzSettings->dontCloseWithOneTab);
    ui->askWhenClosingMultipleTabs->setChecked(qzSettings->askOnClosing);
    ui->showClosedTabsButton->setChecked(qzSettings->showClosedTabsButton);
    ui->showCloseOnInactive->setCurrentIndex(qzSettings->showCloseOnInactiveTabs);
    //AddressBar
    ui->addressbarCompletion->setCurrentIndex(qzSettings->showLocationSuggestions);
    ui->useInlineCompletion->setChecked(qzSettings->useInlineCompletion);
    ui->completionShowSwitchTab->setChecked(qzSettings->showSwitchTab);
    ui->alwaysShowGoIcon->setChecked(qzSettings->alwaysShowGoIcon);
    ui->selectAllOnFocus->setChecked(qzSettings->selectAllOnDoubleClick);
    ui->selectAllOnClick->setChecked(qzSettings->selectAllOnClick);
    bool showPBinAB = qzSettings->showLoadingProgress;
    ui->progressStyleSelector->setCurrentIndex(qzSettings->progressStyle);
    bool pbInABuseCC = qzSettings->useCustomProgressColor;
    QColor pbColor = qzSettings->customProgressColor;

    ui->showLoadingInAddressBar->setChecked(showPBinAB);
    ui->adressProgressSettings->setEnabled(showPBinAB);
    ui->checkBoxCustomProgressColor->setChecked(pbInABuseCC);
    ui->progressBarColorSelector->setEnabled(pbInABuseCC);
    setProgressBarColorIcon(pbColor);
    connect(ui->customColorToolButton, &QAbstractButton::clicked, this, &Preferences::selectCustomProgressBarColor);
    connect(ui->resetProgressBarcolor, SIGNAL(clicked()), SLOT(setProgressBarColorIcon()));

    bool searchFromAB = qzSettings->searchFromAddressBar;
    ui->searchWithDefaultEngine->setChecked(qzSettings->searchWithDefaultEngine);
    ui->showABSearchSuggestions->setChecked(qzSettings->showABSearchSuggestions);

    ui->searchFromAddressBar->setChecked(searchFromAB);
    ui->searchWithDefaultEngine->setEnabled(searchFromAB);
    ui->showABSearchSuggestions->setEnabled(searchFromAB);
    connect(ui->searchFromAddressBar, &QAbstractButton::toggled, this, &Preferences::searchFromAddressBarChanged);
    const QList<int> levels = WebView::zoomLevels();
    for (int level : levels) {
        ui->defaultZoomLevel->addItem(QString("%1%").arg(level));
    }
    // BROWSING
    ui->allowPlugins->setChecked(qzSettings->pluginsEnabled);
    ui->allowJavaScript->setChecked(qzSettings->javascriptEnabled);
    ui->linksInFocusChain->setChecked(qzSettings->linksIncludedInFocusChain);
    ui->spatialNavigation->setChecked(qzSettings->spatialNavigationEnabled);
    ui->animateScrolling->setChecked(qzSettings->scrollAnimatorEnabled);
    ui->wheelScroll->setValue(qzSettings->wheelScrollLines);
    ui->printEBackground->setChecked(qzSettings->printElementBackgrounds);
    ui->useNativeScrollbars->setChecked(qzSettings->useNativeScrollbars);
    ui->disableVideoAutoPlay->setChecked(qzSettings->disableVideoAutoPlay);
    ui->webRTCPublicIpOnly->setChecked(qzSettings->webRTCPublicIpOnly);
    ui->dnsPrefetch->setChecked(qzSettings->dNSPrefetch);
    ui->intPDFViewer->setChecked(qzSettings->intPDFViewer);
    ui->defaultZoomLevel->setCurrentIndex(qzSettings->defaultZoomLevel);
    ui->closeAppWithCtrlQ->setChecked(qzSettings->closeAppWithCtrlQ);
    //Cache
    ui->allowCache->setChecked(qzSettings->allowLocalCache);
    ui->removeCache->setChecked(qzSettings->deleteCacheOnClose);
    ui->cacheMB->setValue(qzSettings->localCacheSize);
    ui->cachePath->setText(settings.value("Web-Browser-Settings/CachePath", QWebEngineProfile::defaultProfile()->cachePath()).toString());
    //PASSWORD MANAGER
    ui->allowPassManager->setChecked(qzSettings->savePasswordsOnSites);
    ui->autoCompletePasswords->setChecked(qzSettings->autoCompletePasswords);

    //PRIVACY

    //Web storage
    ui->saveHistory->setChecked(qzSettings->allowHistory);
    ui->deleteHistoryOnClose->setChecked(qzSettings->deleteHistoryOnClose);
    // Html5Storage
    ui->html5storage->setChecked(qzSettings->localStorageEnabled);
    ui->deleteHtml5storageOnClose->setChecked(qzSettings->deleteHTML5StorageOnClose);
    // Other
    ui->doNotTrack->setChecked(qzSettings->doNotTrack);
    ui->httpsByDefault->setChecked(qzSettings->httpsByDefault);
    //CSS Style
    ui->userStyleSheet->setText(qzSettings->userStyleSheet);
    //NEW
    ui->fullScreenSupportEnabled->setChecked(qzSettings->fullScreenSupportEnabled);
    ui->localContentCanAccessRemoteUrls->setChecked(qzSettings->localContentCanAccessRemoteUrls);
    ui->localContentCanAccessFileUrls->setChecked(qzSettings->localContentCanAccessFileUrls);
    ui->screenCaptureEnabled->setChecked(qzSettings->screenCaptureEnabled);
    ui->touchIconsEnabled->setChecked(qzSettings->touchIconsEnabled);
    ui->allowAllUrlSchemes->setChecked(qzSettings->allowAllUrlSchemes);
    ui->userInteractUrlSchemes->setChecked(qzSettings->userInteractUrlSchemes);
    ui->allowPersistentCookies->setChecked(qzSettings->allowPersistentCookies);

    ui->intPDFViewer->setEnabled(ui->allowPlugins->isChecked());
    connect(ui->html5storage, &QAbstractButton::toggled, this, &Preferences::allowHtml5storageChanged);
    connect(ui->saveHistory, &QAbstractButton::toggled, this, &Preferences::saveHistoryChanged);
    if (!ui->saveHistory->isChecked()) {
        ui->deleteHistoryOnClose->setEnabled(false);
    }
    connect(ui->allowPlugins, &QAbstractButton::toggled, this, &Preferences::allowPluginsToggled);
    connect(ui->allowCache, &QAbstractButton::clicked, this, &Preferences::allowCacheChanged);
    connect(ui->changeCachePath, &QAbstractButton::clicked, this, &Preferences::changeCachePathClicked);
    allowCacheChanged(ui->allowCache->isChecked());
    connect(ui->chooseUserStylesheet, &QAbstractButton::clicked, this, &Preferences::chooseUserStyleClicked);
    //DOWNLOADS
    ui->downLoc->setText(qzSettings->defaultDownloadPath);
    ui->closeDownManOnFinish->setChecked(qzSettings->closeManagerOnFinish);
    ui->useExternalDownManager->setChecked(qzSettings->useExternalManager);
    ui->externalDownExecutable->setText(qzSettings->externalManagerExecutable);
    ui->externalDownArguments->setText(qzSettings->externalManagerArguments);

    if (ui->downLoc->text().isEmpty()) {
        ui->askEverytime->setChecked(true);
    }
    else {
        ui->useDefined->setChecked(true);
    }
    connect(ui->useExternalDownManager, &QAbstractButton::toggled, this, &Preferences::useExternalDownManagerChanged);
    connect(ui->useDefined, &QAbstractButton::toggled, this, &Preferences::downLocChanged);
    connect(ui->downButt, &QAbstractButton::clicked, this, &Preferences::chooseDownPath);
    connect(ui->chooseExternalDown, &QAbstractButton::clicked, this, &Preferences::chooseExternalDownloadManager);
    downLocChanged(ui->useDefined->isChecked());
    useExternalDownManagerChanged(ui->useExternalDownManager->isChecked());
    QWebEngineSettings* webSettings = mApp->webSettings();
    std::function<QString(QWebEngineSettings::FontFamily font)> defaultFont = [&](QWebEngineSettings::FontFamily font) -> const QString {
        const QString family = webSettings->fontFamily(font);
        if (!family.isEmpty())
            return family;
        switch (font) {
        case QWebEngineSettings::FixedFont:
            return QFontDatabase::systemFont(QFontDatabase::FixedFont).family();
        case QWebEngineSettings::SerifFont:
            // TODO
        default:
            return QFontDatabase::systemFont(QFontDatabase::GeneralFont).family();
        }
    };
    //FONTS
    settings.beginGroup("Browser-Fonts");
#if defined(Q_OS_WIN)
    ui->fontStandard->setCurrentFont(QFont(qzSettings->standardFont));
    ui->fontFixed->setCurrentFont(QFont(qzSettings->fixedFont));
    ui->fontSerif->setCurrentFont(QFont(qzSettings->serifFont));
    ui->fontSansSerif->setCurrentFont(QFont(qzSettings->sansSerifFont));
    ui->fontCursive->setCurrentFont(QFont(qzSettings->cursiveFont));
    ui->fontFantasy->setCurrentFont(QFont(qzSettings->fantasyFont));
    ui->fontPictograph->setCurrentFont(QFont(qzSettings->pictographFont));
#elif defined(Q_OS_LINUX)
    ui->fontStandard->setCurrentFont(QFont(qzSettings->standardFont));
    ui->fontFixed->setCurrentFont(QFont(qzSettings->fixedFont));
    ui->fontSerif->setCurrentFont(QFont(qzSettings->serifFont));
    ui->fontSansSerif->setCurrentFont(QFont(qzSettings->sansSerifFont));
    ui->fontCursive->setCurrentFont(QFont(settings.value("CursiveFont", defaultFont(QWebEngineSettings::CursiveFont)).toString()));
    ui->fontFantasy->setCurrentFont(QFont(settings.value("FantasyFont", defaultFont(QWebEngineSettings::FantasyFont)).toString()));
    ui->fontPictograph->setCurrentFont(QFont(settings.value("PictographFont", defaultFont(QWebEngineSettings::PictographFont)).toString()));
#else
    ui->fontStandard->setCurrentFont(QFont(settings.value("StandardFont", defaultFont(QWebEngineSettings::StandardFont)).toString()));
    ui->fontFixed->setCurrentFont(QFont(settings.value("FixedFont", defaultFont(QWebEngineSettings::FixedFont)).toString()));
    ui->fontSerif->setCurrentFont(QFont(settings.value("SerifFont", defaultFont(QWebEngineSettings::SerifFont)).toString()));
    ui->fontSansSerif->setCurrentFont(QFont(settings.value("SansSerifFont", defaultFont(QWebEngineSettings::SansSerifFont)).toString()));
    ui->fontCursive->setCurrentFont(QFont(settings.value("CursiveFont", defaultFont(QWebEngineSettings::CursiveFont)).toString()));
    ui->fontFantasy->setCurrentFont(QFont(settings.value("FantasyFont", defaultFont(QWebEngineSettings::FantasyFont)).toString()));
    ui->fontPictograph->setCurrentFont(QFont(settings.value("PictographFont", defaultFont(QWebEngineSettings::PictographFont)).toString()));
#endif
    ui->sizeDefault->setValue(qzSettings->defaultFontSize);
    ui->sizeFixed->setValue(qzSettings->fixedFontSize);
    ui->sizeMinimum->setValue(qzSettings->minimumFontSize);
    ui->sizeMinimumLogical->setValue(qzSettings->minimumLogicalFontSize);
    //KEYBOARD SHORTCUTS
    settings.beginGroup("Shortcuts");
    ui->switchTabsAlt->setChecked(qzSettings->useTabNumberShortcuts);
    ui->loadSpeedDialsCtrl->setChecked(qzSettings->useSpeedDialNumberShortcuts);
    ui->singleKeyShortcuts->setChecked(qzSettings->useSingleKeyShortcuts);
    //NOTIFICATIONS
#if defined(Q_OS_LINUX)
        ui->useNativeSystemNotifications->setEnabled(mApp->desktopNotifications()->supportsNativeNotifications());
#endif

    DesktopNotificationsFactory::Type notifyType;

    ui->notificationTimeout->setValue(qzSettings->timeout);
#if defined(Q_OS_LINUX) && !defined(DISABLE_DBUS)
    notifyType = qzSettings->useNativeDesktop ? DesktopNotificationsFactory::DesktopNative : DesktopNotificationsFactory::PopupWidget;
#else
    notifyType = DesktopNotificationsFactory::PopupWidget;
#endif
    ui->doNotUseNotifications->setChecked(!qzSettings->notificationsEnabled);
    m_notifPosition = qzSettings->position;

#if defined(Q_OS_LINUX)
    if (ui->useNativeSystemNotifications->isEnabled() && notifyType == DesktopNotificationsFactory::DesktopNative) {
        ui->useNativeSystemNotifications->setChecked(true);
    } else {
        ui->useOSDNotifications->setChecked(true);
    }
#else
    ui->useOSDNotifications->setChecked(true);
    ui->useNativeSystemNotifications->setHidden(true);
#endif
    connect(ui->notificationPreview, &QPushButton::clicked, this, &Preferences::showNotificationPreview);
    //SPELLCHECK
    ui->spellcheckEnabled->setChecked(qzSettings->enabled);
    const QStringList spellcheckLanguages = qzSettings->languages;

    std::function<void()> updateSpellCheckEnabled = [this]() {
        ui->spellcheckLanguages->setEnabled(ui->spellcheckEnabled->isChecked());
        ui->spellcheckNoLanguages->setEnabled(ui->spellcheckEnabled->isChecked());
    };
    updateSpellCheckEnabled();
    connect(ui->spellcheckEnabled, &QCheckBox::toggled, this, updateSpellCheckEnabled);

    QStringList dictionariesDirs;

    dictionariesDirs = QStringList {
#ifdef Q_OS_OSX
    QDir::cleanPath(QCoreApplication::applicationDirPath() + QLatin1String("/../Resources/qtwebengine_dictionaries")),
    QDir::cleanPath(QCoreApplication::applicationDirPath() + QLatin1String("/../Frameworks/QtWebEngineCore.framework/Resources/qtwebengine_dictionaries"))
#elif defined(Q_OS_LINUX)
    QDir::cleanPath(DataPaths::currentProfilePath() + QLatin1String("/qtwebengine_dictionaries")), // Needs testing
    QDir::cleanPath(QLibraryInfo::location(QLibraryInfo::DataPath) + QLatin1String("/qtwebengine_dictionaries"))
#elif defined(Q_OS_WIN)
    QDir::cleanPath(QCoreApplication::applicationDirPath() + QLatin1String("/qtwebengine_dictionaries"))
#else
    QDir::cleanPath(QCoreApplication::applicationDirPath() + QLatin1String("/qtwebengine_dictionaries")),
    QDir::cleanPath(DataPaths::currentProfilePath() + QLatin1String("/qtwebengine_dictionaries")), // Needs testing
    QDir::cleanPath(QLibraryInfo::location(QLibraryInfo::DataPath) + QLatin1String("/qtwebengine_dictionaries"))
#endif
    };

    dictionariesDirs.removeDuplicates();

    ui->spellcheckDirectories->setText(dictionariesDirs.join(QLatin1Char('\n')));
    for (const QString &path : dictionariesDirs) {
        QDir dir(path);
        const QStringList files = dir.entryList({QStringLiteral("*.bdic")});
        for (const QString &file : files) {
            const QString lang = file.left(file.size() - 5);
            const QString langName = createLanguageItem(lang);
            if (!ui->spellcheckLanguages->findItems(langName, Qt::MatchExactly).isEmpty()) {
                continue;
            }
            QListWidgetItem *item = new QListWidgetItem;
            item->setText(langName);
            item->setData(Qt::UserRole, lang);
            item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
            item->setCheckState(Qt::Unchecked);
            ui->spellcheckLanguages->addItem(item);
        }
    }

	int n = 2;
    int topIndex = 0;
    for (const QString &lang : spellcheckLanguages) {
        const QList<QListWidgetItem *> items = ui->spellcheckLanguages->findItems(createLanguageItem(lang), Qt::MatchExactly);
        if (items.isEmpty()) {
            continue;
        }
        QListWidgetItem *item = items.at(0);
        ui->spellcheckLanguages->takeItem(ui->spellcheckLanguages->row(item));
        ui->spellcheckLanguages->insertItem(topIndex++, item);
		if (n == 2) {
			item->setCheckState(Qt::Checked);
			n++;
		} else {
			item->setCheckState(Qt::Unchecked);
		}
    }

    if (ui->spellcheckLanguages->count() == 0) {
        ui->spellcheckLanguages->hide();
    } else {
        ui->spellcheckNoLanguages->hide();
    }
    // Proxy Configuration
    int proxyType = qzSettings->proxyType;
    if (proxyType == 0) {
        ui->noProxy->setChecked(true);
    } else if (proxyType == 2) {
        ui->systemProxy->setChecked(true);
    } else if (proxyType == 3) {
        ui->manualProxy->setChecked(true);
        ui->proxyType->setCurrentIndex(0);
    } else {
        ui->manualProxy->setChecked(true);
        ui->proxyType->setCurrentIndex(1);
    }

    ui->proxyServer->setText(qzSettings->hostName);
    ui->proxyPort->setText(qzSettings->port);
    ui->proxyUsername->setText(qzSettings->username);
    ui->proxyPassword->setText(qzSettings->password);

    setManualProxyConfigurationEnabled(ui->manualProxy->isChecked());
    //CONNECTS
    connect(ui->manualProxy, &QAbstractButton::toggled, this, &Preferences::setManualProxyConfigurationEnabled);
    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &Preferences::buttonClicked);
    connect(ui->cookieManagerBut, &QAbstractButton::clicked, this, &Preferences::showCookieManager);
    connect(ui->html5permissions, &QAbstractButton::clicked, this, &Preferences::showHtml5Permissions);
    connect(ui->preferredLanguages, &QAbstractButton::clicked, this, &Preferences::showAcceptLanguage);
    connect(ui->deleteHtml5storage, &QAbstractButton::clicked, this, &Preferences::deleteHtml5storage);
    connect(ui->uaManager, &QAbstractButton::clicked, this, &Preferences::openUserAgentManager);
    connect(ui->jsOptionsButton, &QAbstractButton::clicked, this, &Preferences::openJsOptions);
    connect(ui->searchEngines, &QAbstractButton::clicked, this, &Preferences::openSearchEnginesManager);
    connect(ui->protocolHandlers, &QAbstractButton::clicked, this, &Preferences::openProtocolHandlersManager);
    connect(ui->listWidget, &QListWidget::currentItemChanged, this, &Preferences::showStackedPage);
    connect(ui->preferencesReset, &QAbstractButton::clicked, this, &Preferences::prefReset);

    ui->listWidget->itemAt(5, 5)->setSelected(true);
    ui->listWidget->setCurrentRow(currentSettingsPage);

    QDesktopWidget* desktop = QApplication::desktop();
    QSize s = size();
    if (desktop->availableGeometry(this).size().width() < s.width()) {
        s.setWidth(desktop->availableGeometry(this).size().width() - 50);
    }
    if (desktop->availableGeometry(this).size().height() < s.height()) {
        s.setHeight(desktop->availableGeometry(this).size().height() - 50);
    }
    resize(s);

    settings.beginGroup(QStringLiteral("Preferences"));
    restoreGeometry(qzSettings->geometry);
    settings.endGroup();

    QzTools::setWmClass("Preferences", this);

}

bool Preferences::prefReset()
{
  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(this, tr("Reset"), tr("Are you sure you want to reset all preferences to their defaults?\n(This action cannot be undone and the changes will not occur until the browser is restarted.)"), QMessageBox::Yes|QMessageBox::No);
  if (reply == QMessageBox::Yes) {
    Settings settings;
    settings.setValue(QStringLiteral("Web-Browser-Settings/resetPrefs"), true);
    return true;
  } else {
    return false;
  }

}

void Preferences::allowPluginsToggled(bool checked)
{
    ui->intPDFViewer->setEnabled(checked);
}

void Preferences::chooseExternalDownloadManager()
{
    QString path = QzTools::getOpenFileName("Preferences-ExternalDownloadManager", this, tr("Choose executable location..."), QDir::homePath());
    if (path.isEmpty()) {
        return;
    }

    ui->externalDownExecutable->setText(path);
}

void Preferences::showStackedPage(QListWidgetItem* item)
{
    if (!item) {
        return;
    }

    int index = ui->listWidget->currentRow();

    ui->caption->setText("<b>" + item->text() + "</b>");
    ui->stackedWidget->setCurrentIndex(index);

    if (m_notification) {
        m_notifPosition = m_notification.data()->pos();
        delete m_notification.data();
    }

    if (index == 10) {
        m_pluginsList->load();
    }

    if (index == 7 && !m_autoFillManager) {
        m_autoFillManager = new AutoFillManager(this);
        ui->autoFillFrame->addWidget(m_autoFillManager);
    }
}

void Preferences::showNotificationPreview()
{
    if (ui->useOSDNotifications->isChecked()) {
        if (m_notification) {
            m_notifPosition = m_notification.data()->pos();
            delete m_notification.data();
        }

        m_notification = new DesktopNotification(true);
        m_notification.data()->setHeading(tr("OSD Notification"));
        m_notification.data()->setText(tr("Drag it on the screen to place it where you want."));
        m_notification.data()->move(m_notifPosition);
        m_notification.data()->show();
    }
    else if (ui->useNativeSystemNotifications->isChecked()) {
        mApp->desktopNotifications()->nativeNotificationPreview();
    }
}

void Preferences::allowCacheChanged(bool state)
{
    ui->removeCache->setEnabled(state);
    ui->maxCacheLabel->setEnabled(state);
    ui->cacheMB->setEnabled(state);
    ui->storeCacheLabel->setEnabled(state);
    ui->cachePath->setEnabled(state);
    ui->changeCachePath->setEnabled(state);
}

void Preferences::useActualHomepage()
{
    if (!m_window)
        return;

    ui->homepage->setText(m_window->weView()->url().toString());
}

void Preferences::useActualNewTab()
{
    if (!m_window)
        return;

    ui->newTabUrl->setText(m_window->weView()->url().toString());
}

void Preferences::chooseDownPath()
{
    QString userFileName = QzTools::getExistingDirectory("Preferences-ChooseDownPath", this, tr("Choose download location..."), QDir::homePath());
    if (userFileName.isEmpty()) {
        return;
    }
#ifdef Q_OS_WIN   //QFileDialog::getExistingDirectory returns path with \ instead of / (??)
    userFileName.replace(QLatin1Char('\\'), QLatin1Char('/'));
#endif
    userFileName += QLatin1Char('/');

    ui->downLoc->setText(userFileName);
}

void Preferences::chooseUserStyleClicked()
{
    QString file = QzTools::getOpenFileName("Preferences-UserStyle", this, tr("Choose stylesheet location..."), QDir::homePath(), "*.css");
    if (file.isEmpty()) {
        return;
    }
    ui->userStyleSheet->setText(file);
}

void Preferences::deleteHtml5storage()
{
    ClearPrivateData::clearLocalStorage();

    ui->deleteHtml5storage->setText(tr("Deleted"));
    ui->deleteHtml5storage->setEnabled(false);
}

void Preferences::openUserAgentManager()
{
    UserAgentDialog* dialog = new UserAgentDialog(this);
    dialog->open();
}

void Preferences::downLocChanged(bool state)
{
    ui->downButt->setEnabled(state);
    ui->downLoc->setEnabled(state);
}

void Preferences::setManualProxyConfigurationEnabled(bool state)
{
    ui->proxyType->setEnabled(state);
    ui->proxyServer->setEnabled(state);
    ui->proxyPort->setEnabled(state);
    ui->proxyUsername->setEnabled(state);
    ui->proxyPassword->setEnabled(state);
}

void Preferences::searchFromAddressBarChanged(bool stat)
{
    ui->searchWithDefaultEngine->setEnabled(stat);
    ui->showABSearchSuggestions->setEnabled(stat);
}

void Preferences::saveHistoryChanged(bool stat)
{
    ui->deleteHistoryOnClose->setEnabled(stat);
}

void Preferences::allowHtml5storageChanged(bool stat)
{
    ui->deleteHtml5storageOnClose->setEnabled(stat);
}

void Preferences::showCookieManager()
{
    CookieManager* dialog = new CookieManager(this);
    dialog->show();
}

void Preferences::showHtml5Permissions()
{
    HTML5PermissionsDialog* dialog = new HTML5PermissionsDialog(this);
    dialog->open();
}

void Preferences::openJsOptions()
{
    JsOptions* dialog = new JsOptions(this);
    dialog->open();
}

void Preferences::useExternalDownManagerChanged(bool state)
{
    ui->externalDownExecutable->setEnabled(state);
    ui->externalDownArguments->setEnabled(state);
    ui->chooseExternalDown->setEnabled(state);
}

void Preferences::openSearchEnginesManager()
{
    SearchEnginesDialog* dialog = new SearchEnginesDialog(this);
    dialog->open();
}

void Preferences::openProtocolHandlersManager()
{
    ProtocolHandlerDialog *dialog = new ProtocolHandlerDialog(this);
    dialog->open();
}

void Preferences::showAcceptLanguage()
{
    AcceptLanguage* dialog = new AcceptLanguage(this);
    dialog->open();
}

void Preferences::newTabChanged(int value)
{
    ui->newTabFrame->setVisible(value == 3);
}

void Preferences::afterLaunchChanged(int value)
{
    ui->dontLoadTabsUntilSelected->setEnabled(value == 3 || value == 4);
}

void Preferences::changeCachePathClicked()
{
    QString path = QzTools::getExistingDirectory("Preferences-CachePath", this, tr("Choose cache path..."), ui->cachePath->text());
    if (path.isEmpty()) {
        return;
    }

    ui->cachePath->setText(path);
}

void Preferences::buttonClicked(QAbstractButton* button)
{
    switch (ui->buttonBox->buttonRole(button)) {
    case QDialogButtonBox::ApplyRole:
        saveSettings();
        break;

    case QDialogButtonBox::RejectRole:
        close();
        break;

    case QDialogButtonBox::AcceptRole:
        saveSettings();
        close();
        break;

    default:
        break;
    }
}

void Preferences::createProfile()
{
    QString name = QInputDialog::getText(this, tr("New Profile"), tr("Enter the new profile's name:"));
    name = QzTools::filterCharsFromFilename(name);

    if (name.isEmpty()) {
        return;
    }

    int res = ProfileManager::createProfile(name);

    if (res == -1) {
        QMessageBox::warning(this, tr("Error!"), tr("This profile already exists!"));
        return;
    }

    if (res != 0) {
        QMessageBox::warning(this, tr("Error!"), tr("Cannot create profile directory!"));
        return;
    }

    ui->startProfile->addItem(name);
    ui->startProfile->setCurrentIndex(ui->startProfile->count() - 1);
}

void Preferences::deleteProfile()
{
    QString name = ui->startProfile->currentText();
    QMessageBox::StandardButton button = QMessageBox::warning(this, tr("Confirmation"),
                                         tr("Are you sure you want to permanently delete \"%1\" profile? This action cannot be undone!").arg(name), QMessageBox::Yes | QMessageBox::No);
    if (button != QMessageBox::Yes) {
        return;
    }

    ProfileManager::removeProfile(name);

    ui->startProfile->removeItem(ui->startProfile->currentIndex());
}

void Preferences::startProfileIndexChanged(int index)
{
    const bool current = ui->startProfile->itemText(index) == ProfileManager::currentProfile();

    ui->deleteProfile->setEnabled(!current);
    ui->cannotDeleteActiveProfileLabel->setText(current ? tr("Note: You cannot delete the active profile.") : QString());
}

void Preferences::closeEvent(QCloseEvent* event)
{
    Settings settings;
    settings.setValue("Browser-View-Settings/settingsDialogPage", ui->stackedWidget->currentIndex());

    event->accept();
}

void Preferences::saveSettings()
{
    Settings settings;

    QUrl homepage = QUrl::fromUserInput(ui->homepage->text());

    qzSettings->homepage = homepage;
    qzSettings->afterlaunch = ui->afterLaunch->currentIndex();

    switch (ui->newTab->currentIndex()) {

    case 0:
        qzSettings->newTabUrl = QUrl();
        break;

    case 1:
        qzSettings->newTabUrl = homepage;
        break;

    case 2:
        qzSettings->newTabUrl = QUrl(QStringLiteral("browser:speeddial"));
        break;

    case 3:
        qzSettings->newTabUrl = QUrl::fromUserInput(ui->newTabUrl->text());
        break;

    default:
        break;
    }

    qzSettings->showSearchBar = ui->showSearchbar->isChecked();

    qzSettings->showStatusBar = ui->showStatusbar->isChecked();
    qzSettings->instantBookmarksToolbar = ui->instantBookmarksToolbar->isChecked();
    qzSettings->showBookmarksToolbar = ui->showBookmarksToolbar->isChecked();
    qzSettings->showNavigationToolbar = ui->showNavigationToolbar->isChecked();

    if (ui->showNavigationToolbar->isChecked()) {
        qzSettings->showMenuBar = false;
    }

    qzSettings->showNavInFullScreen = ui->navInFullScreen->isChecked();
    qzSettings->fullScreenLocationBarAutoHide = ui->locBarAutoHide->isChecked();

    qzSettings->newTabPos = ui->switchToNewTabs->isChecked();    
    qzSettings->newTabPosition = ui->switchToNewTabs->isChecked() ? Qz::NT_CleanSelectedTab : Qz::NT_CleanNotSelectedTab;
    qzSettings->openPopupsInTabs = ui->openPopupsInTabs->isChecked();
    qzSettings->alwaysSwitchTabsWithWheel = ui->alwaysSwitchTabsWithWheel->isChecked();
    qzSettings->hideTabsWithOneTab = ui->hideTabsOnTab->isChecked();
    qzSettings->activateLastTabWhenClosingActual = ui->activateLastTab->isChecked();
    qzSettings->newTabAfterActive = ui->openNewTabAfterActive->isChecked();
    qzSettings->newEmptyTabAfterActive = ui->openNewEmptyTabAfterActive->isChecked();
    qzSettings->dontCloseWithOneTab = ui->dontCloseOnLastTab->isChecked();
    qzSettings->askOnClosing = ui->askWhenClosingMultipleTabs->isChecked();
    qzSettings->showClosedTabsButton = ui->showClosedTabsButton->isChecked();
    qzSettings->showCloseOnInactiveTabs = ui->showCloseOnInactive->currentIndex();

    if (ui->askEverytime->isChecked()) {
        qzSettings->defaultDownloadPath = "";
    } else {
        qzSettings->defaultDownloadPath = ui->downLoc->text();
    }

    qzSettings->closeManagerOnFinish = ui->closeDownManOnFinish->isChecked();
    qzSettings->useExternalManager = ui->useExternalDownManager->isChecked();
    qzSettings->externalManagerExecutable = ui->externalDownExecutable->text();
    qzSettings->externalManagerArguments = ui->externalDownArguments->text();

    qzSettings->standardFont = ui->fontStandard->currentFont().family();
    qzSettings->fixedFont = ui->fontFixed->currentFont().family();
    qzSettings->serifFont = ui->fontSerif->currentFont().family();
    qzSettings->sansSerifFont = ui->fontSansSerif->currentFont().family();
    qzSettings->cursiveFont = ui->fontCursive->currentFont().family();
    qzSettings->fantasyFont = ui->fontFantasy->currentFont().family();
    qzSettings->pictographFont = ui->fontPictograph->currentFont().family();
    qzSettings->defaultFontSize = ui->sizeDefault->value();
    qzSettings->fixedFontSize = ui->sizeFixed->value();
    qzSettings->minimumFontSize = ui->sizeMinimum->value();
    qzSettings->minimumLogicalFontSize = ui->sizeMinimumLogical->value();

    qzSettings->useTabNumberShortcuts = ui->switchTabsAlt->isChecked();
    qzSettings->useSpeedDialNumberShortcuts = ui->loadSpeedDialsCtrl->isChecked();
    qzSettings->useSingleKeyShortcuts = ui->singleKeyShortcuts->isChecked();

    qzSettings->useNativeScrollbars = ui->useNativeScrollbars->isChecked();
    qzSettings->closeAppWithCtrlQ = ui->closeAppWithCtrlQ->isChecked();
    qzSettings->allowLocalCache = ui->allowCache->isChecked();
    qzSettings->deleteCacheOnClose = ui->removeCache->isChecked();
    qzSettings->localCacheSize = ui->cacheMB->value();
    qzSettings->cachePath = ui->cachePath->text();
    qzSettings->savePasswordsOnSites = ui->allowPassManager->isChecked();
    qzSettings->autoCompletePasswords = ui->autoCompletePasswords->isChecked();
    qzSettings->allowHistory = ui->saveHistory->isChecked();
    qzSettings->deleteHistoryOnClose = ui->deleteHistoryOnClose->isChecked();
    qzSettings->deleteHTML5StorageOnClose = ui->deleteHtml5storageOnClose->isChecked();
    qzSettings->doNotTrack = ui->doNotTrack->isChecked();
    qzSettings->defaultZoomLevel = ui->defaultZoomLevel->currentIndex();
    qzSettings->loadTabsOnActivation = ui->dontLoadTabsUntilSelected->isChecked();
    qzSettings->httpsByDefault = ui->httpsByDefault->isChecked();
    qzSettings->localStorageEnabled = ui->html5storage->isChecked();
    qzSettings->pluginsEnabled = ui->allowPlugins->isChecked();
    qzSettings->linksIncludedInFocusChain = ui->linksInFocusChain->isChecked();
    qzSettings->printElementBackgrounds = ui->printEBackground->isChecked();
    qzSettings->spatialNavigationEnabled = ui->spatialNavigation->isChecked();
    qzSettings->scrollAnimatorEnabled = ui->animateScrolling->isChecked();
    qzSettings->fullScreenSupportEnabled = ui->fullScreenSupportEnabled->isChecked();
    qzSettings->localContentCanAccessRemoteUrls = ui->localContentCanAccessRemoteUrls->isChecked();
    qzSettings->localContentCanAccessFileUrls = ui->localContentCanAccessFileUrls->isChecked();
    qzSettings->screenCaptureEnabled = ui->screenCaptureEnabled->isChecked();
    qzSettings->touchIconsEnabled = ui->touchIconsEnabled->isChecked();
    qzSettings->allowJavaScriptPaste = ui->allowJavaScript->isChecked();
    qzSettings->disableVideoAutoPlay = ui->disableVideoAutoPlay->isChecked();
    qzSettings->webRTCPublicIpOnly = ui->webRTCPublicIpOnly->isChecked();
    qzSettings->dNSPrefetch = ui->dnsPrefetch->isChecked();
    qzSettings->intPDFViewer = ui->intPDFViewer->isChecked();
    qzSettings->wheelScrollLines = ui->wheelScroll->value();
    qzSettings->userStyleSheet = ui->userStyleSheet->text();
    qzSettings->checkUpdates = ui->checkUpdates->isChecked();
    qzSettings->allowAllUrlSchemes = ui->allowAllUrlSchemes->isChecked();
    qzSettings->userInteractUrlSchemes = ui->userInteractUrlSchemes->isChecked();
    qzSettings->allowPersistentCookies = ui->allowPersistentCookies->isChecked();
#ifdef Q_OS_WIN
    qzSettings->checkDefaultBrowser = ui->checkDefaultBrowser->isChecked();
#endif

    qzSettings->timeout = ui->notificationTimeout->value() * 1000;
#if defined(Q_OS_LINUX) && !defined(DISABLE_DBUS)
    ui->useNativeSystemNotifications->isChecked();
#endif
    qzSettings->notificationsEnabled = !ui->doNotUseNotifications->isChecked();
    qzSettings->position = m_notification.data() ? m_notification.data()->pos() : m_notifPosition;

    QStringList languages;

    for (int i = 0; i < ui->spellcheckLanguages->count(); ++i) {
        QListWidgetItem *item = ui->spellcheckLanguages->item(i);
        if (item->checkState() == Qt::Checked) {
            languages.append(item->data(Qt::UserRole).toString());
        }
    }

    qzSettings->languages = languages;
    qzSettings->enabled = ui->spellcheckEnabled->isChecked();

    qzSettings->selectAllOnDoubleClick = ui->selectAllOnFocus->isChecked();
    qzSettings->selectAllOnClick = ui->selectAllOnClick->isChecked();
    qzSettings->showLoadingProgress = ui->showLoadingInAddressBar->isChecked();
    qzSettings->showLocationSuggestions = ui->addressbarCompletion->currentIndex();
    qzSettings->showSwitchTab = ui->completionShowSwitchTab->isChecked();
    qzSettings->alwaysShowGoIcon = ui->alwaysShowGoIcon->isChecked();
    qzSettings->useInlineCompletion = ui->useInlineCompletion->isChecked();
    qzSettings->progressStyle = ui->progressStyleSelector->currentIndex();
    qzSettings->useCustomProgressColor = ui->checkBoxCustomProgressColor->isChecked();
    qzSettings->customProgressColor = ui->customColorToolButton->property("ProgressColor").value<QColor>();

    qzSettings->searchFromAddressBar = ui->searchFromAddressBar->isChecked();
    qzSettings->searchWithDefaultEngine = ui->searchWithDefaultEngine->isChecked();
    qzSettings->showABSearchSuggestions = ui->showABSearchSuggestions->isChecked();

    int proxyType;

    if (ui->noProxy->isChecked()) {
        proxyType = 0;
    } else if (ui->systemProxy->isChecked()) {
        proxyType = 2;
    } else if (ui->proxyType->currentIndex() == 0) { // Http
        proxyType = 3;
    } else { // Socks5
        proxyType = 4;
    }

    qzSettings->proxyType = proxyType;
    qzSettings->hostName = ui->proxyServer->text();
    qzSettings->port = ui->proxyPort->text().toInt();
    qzSettings->username = ui->proxyUsername->text();
    qzSettings->password = ui->proxyPassword->text();

    qzSettings->saveSettings();

    ProfileManager::setStartingProfile(ui->startProfile->currentText());

    m_pluginsList->save();
    m_themesManager->save();

    mApp->cookieJar()->loadSettings();
    mApp->history()->loadSettings();
    mApp->desktopNotifications()->loadSettings();
    mApp->autoFill()->loadSettings();
    mApp->networkManager()->loadSettings();
    WebScrollBarManager::instance()->loadSettings();
    mApp->reloadSettings();

}

Preferences::~Preferences()
{
    Settings().setValue(QStringLiteral("Preferences/Geometry"), saveGeometry());

    delete ui;
    delete m_autoFillManager;
    delete m_pluginsList;
    delete m_notification.data();
}

void Preferences::setProgressBarColorIcon(QColor color)
{
    const int size = style()->pixelMetric(QStyle::PM_ToolBarIconSize);
    QPixmap pm(QSize(size, size));
    if (!color.isValid()) {
        color = palette().color(QPalette::Highlight);
    }
    pm.fill(color);
    ui->customColorToolButton->setIcon(pm);
    ui->customColorToolButton->setProperty("ProgressColor", color);
}

void Preferences::selectCustomProgressBarColor()
{
    QColor newColor = QColorDialog::getColor(ui->customColorToolButton->property("ProgressColor").value<QColor>(), this, tr("Select Color"));
    if (newColor.isValid()) {
        setProgressBarColorIcon(newColor);
    }
}
