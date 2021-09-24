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
#include "adblockmanager.h"
#include "adblockdialog.h"
#include "adblockmatcher.h"
#include "adblocksubscription.h"
#include "adblockurlinterceptor.h"
#include "datapaths.h"
#include "mainapplication.h"
#include "webpage.h"
#include "qztools.h"
#include "browserwindow.h"
#include "settings.h"
#include "networkmanager.h"

#include <QAction>
#include <QDateTime>
#include <QTextStream>
#include <QDir>
#include <QTimer>
#include <QMessageBox>
#include <QUrlQuery>
#include <QMutexLocker>
#include <QSaveFile>

//#define ADBLOCK_DEBUG

#ifdef ADBLOCK_DEBUG
#include <QElapsedTimer>
#endif

Q_GLOBAL_STATIC(AdBlockManager, qz_adblock_manager)

AdBlockManager::AdBlockManager(QObject* parent)
    : QObject(parent)
    , m_loaded(false)
    , m_enabled(true)
    , m_matcher(new AdBlockMatcher(this))
    , m_interceptor(new AdBlockUrlInterceptor(this))
{
    qRegisterMetaType<AdBlockedRequest>();

    load();
}

AdBlockManager::~AdBlockManager()
{
    qDeleteAll(m_subscriptions);
}

AdBlockManager* AdBlockManager::instance()
{
    return qz_adblock_manager();
}

void AdBlockManager::setEnabled(bool enabled)
{
    if (m_enabled == enabled) {
        return;
    }

    m_enabled = enabled;
    emit enabledChanged(enabled);

    Settings settings;
    settings.beginGroup(QStringLiteral("AdBlock"));
    settings.setValue(QStringLiteral("enabled"), m_enabled);
    settings.endGroup();

    load();
    mApp->reloadUserStyleSheet();

    QMutexLocker locker(&m_mutex);

    if (m_enabled) {
        m_matcher->update();
    } else {
        m_matcher->clear();
    }
}

QList<AdBlockSubscription*> AdBlockManager::subscriptions() const
{
    return m_subscriptions;
}

bool AdBlockManager::block(QWebEngineUrlRequestInfo &request, QString &ruleFilter, QString &ruleSubscription)
{
    QMutexLocker locker(&m_mutex);

    if (!isEnabled()) {
        return false;
    }

#ifdef ADBLOCK_DEBUG
    QElapsedTimer timer;
    timer.start();
#endif
    const QString urlString = request.requestUrl().toEncoded().toLower();
    const QString urlDomain = request.requestUrl().host().toLower();
    const QString urlScheme = request.requestUrl().scheme().toLower();

    if (!canRunOnScheme(urlScheme) || !canBeBlocked(request.firstPartyUrl())) {
        return false;
    }

    const AdBlockRule* blockedRule = m_matcher->match(request, urlDomain, urlString);

    if (blockedRule) {
        ruleFilter = blockedRule->filter();
        ruleSubscription = blockedRule->subscription()->title();
#ifdef ADBLOCK_DEBUG
        qDebug() << "BLOCKED: " << timer.elapsed() << blockedRule->filter() << request.requestUrl();
#endif
    }

#ifdef ADBLOCK_DEBUG
    qDebug() << timer.elapsed() << request.requestUrl();
#endif

    return blockedRule;
}

QVector<AdBlockedRequest> AdBlockManager::blockedRequestsForUrl(const QUrl &url) const
{
    return m_blockedRequests.value(url);
}

void AdBlockManager::clearBlockedRequestsForUrl(const QUrl &url)
{
    if (m_blockedRequests.remove(url)) {
        emit blockedRequestsChanged(url);
    }
}

QStringList AdBlockManager::disabledRules() const
{
    return m_disabledRules;
}

void AdBlockManager::addDisabledRule(const QString &filter)
{
    m_disabledRules.append(filter);
}

void AdBlockManager::removeDisabledRule(const QString &filter)
{
    m_disabledRules.removeOne(filter);
}

bool AdBlockManager::addSubscriptionFromUrl(const QUrl &url)
{
    const QList<QPair<QString, QString> > queryItems = QUrlQuery(url).queryItems(QUrl::FullyDecoded);

    QString subscriptionTitle;
    QString subscriptionUrl;

    for (int i = 0; i < queryItems.count(); ++i) {
        QPair<QString, QString> pair = queryItems.at(i);
        if (pair.first.endsWith(QLatin1String("location")))
            subscriptionUrl = pair.second;
        else if (pair.first.endsWith(QLatin1String("title")))
            subscriptionTitle = pair.second;
    }

    if (subscriptionTitle.isEmpty() || subscriptionUrl.isEmpty())
        return false;

    const QString message = AdBlockManager::tr("Do you want to add <b>%1</b> subscription?").arg(subscriptionTitle);

    QMessageBox::StandardButton result = QMessageBox::question(nullptr, AdBlockManager::tr("AdBlock Subscription"), message, QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes) {
        AdBlockManager::instance()->addSubscription(subscriptionTitle, subscriptionUrl);
        AdBlockManager::instance()->showDialog();
    }

    return true;
}

AdBlockSubscription* AdBlockManager::addSubscription(const QString &title, const QString &url)
{
    if (title.isEmpty() || url.isEmpty()) {
        return nullptr;
    }

    QString fileName = QzTools::filterCharsFromFilename(title.toLower()) + QStringLiteral(".txt");
    QString filePath = QzTools::ensureUniqueFilename(DataPaths::currentProfilePath() + QStringLiteral("/adblock/") + fileName);

    QByteArray data = QStringLiteral("Title: %1\nUrl: %2\n[Adblock Plus 1.1.1]").arg(title, url).toLatin1();

    QSaveFile file(filePath);
    if (!file.open(QFile::WriteOnly)) {
        qWarning() << "AdBlockManager: Cannot write to file" << filePath;
        return nullptr;
    }
    file.write(data);
    file.commit();

    AdBlockSubscription* subscription = new AdBlockSubscription(title, this);
    subscription->setUrl(QUrl(url));
    subscription->setFilePath(filePath);
    subscription->loadSubscription(m_disabledRules);

    m_subscriptions.insert(m_subscriptions.count() - 1, subscription);
    connect(subscription, &AdBlockSubscription::subscriptionUpdated, mApp, &MainApplication::reloadUserStyleSheet);
    connect(subscription, &AdBlockSubscription::subscriptionChanged, this, &AdBlockManager::updateMatcher);

    return subscription;
}

bool AdBlockManager::removeSubscription(AdBlockSubscription* subscription)
{
    QMutexLocker locker(&m_mutex);

    if (!m_subscriptions.contains(subscription) || !subscription->canBeRemoved()) {
        return false;
    }

    QFile(subscription->filePath()).remove();
    m_subscriptions.removeOne(subscription);

    m_matcher->update();
    delete subscription;

    return true;
}

AdBlockCustomList* AdBlockManager::customList() const
{
    for (AdBlockSubscription* subscription : qAsConst(m_subscriptions)) {
        AdBlockCustomList* list = qobject_cast<AdBlockCustomList*>(subscription);

        if (list) {
            return list;
        }
    }

    return nullptr;
}

void AdBlockManager::load()
{
    QMutexLocker locker(&m_mutex);

    if (m_loaded) {
        return;
    }

#ifdef ADBLOCK_DEBUG
    QElapsedTimer timer;
    timer.start();
#endif

    Settings settings;
    settings.beginGroup(QStringLiteral("AdBlock"));
    m_enabled = settings.value(QStringLiteral("enabled"), m_enabled).toBool();
    m_disabledRules = settings.value(QStringLiteral("disabledRules"), QStringList()).toStringList();
    QDateTime lastUpdate = settings.value(QStringLiteral("lastUpdate"), QDateTime()).toDateTime();
    settings.endGroup();

    if (!m_enabled) {
        return;
    }

    QDir adblockDir(DataPaths::currentProfilePath() + QStringLiteral("/adblock"));
    // Create if necessary
    if (!adblockDir.exists()) {
        QDir(DataPaths::currentProfilePath()).mkdir(QStringLiteral("adblock"));
    }

    const QStringList fileNames = adblockDir.entryList(QStringList(QSL("*.txt")), QDir::Files);
    for (const QString &fileName : fileNames) {
        if (fileName == QLatin1String("customlist.txt")) {
            continue;
        }

        const QString absolutePath = adblockDir.absoluteFilePath(fileName);
        QFile file(absolutePath);
        if (!file.open(QFile::ReadOnly)) {
            continue;
        }

        QTextStream textStream(&file);
        textStream.setCodec("UTF-8");
        QString title = textStream.readLine(1024).remove(QLatin1String("Title: "));
        QUrl url = QUrl(textStream.readLine(1024).remove(QLatin1String("Url: ")));

        if (title.isEmpty() || !url.isValid()) {
            qWarning() << "AdBlockManager: Invalid subscription file. Please remove this subscription." << absolutePath;
            continue;
        }

        AdBlockSubscription* subscription = new AdBlockSubscription(title, this);
        subscription->setUrl(url);
        subscription->setFilePath(absolutePath);

        m_subscriptions.append(subscription);
    }

    // Add subscriptions
    if (m_subscriptions.isEmpty()) {

        AdBlockSubscription *aList = new AdBlockSubscription(QLatin1String("EasyList"), this);
        aList->setUrl(QUrl(ADBLOCK_ELIST_URL));
        aList->setFilePath(DataPaths::currentProfilePath() + QLatin1String("/adblock/aList.txt"));
        m_subscriptions.append(aList);

        AdBlockSubscription *bList = new AdBlockSubscription(QLatin1String("Peter Lowe's list (English)"), this);
        bList->setUrl(QUrl(ADBLOCK_PLIST_URL));
        bList->setFilePath(DataPaths::currentProfilePath() + QLatin1String("/adblock/bList.txt"));
        m_subscriptions.append(bList);

        AdBlockSubscription *cList = new AdBlockSubscription(QLatin1String("ABP Anti-Circumvention Filter List"), this);
        cList->setUrl(QUrl(ADBLOCK_ABPACFL_URL));
        cList->setFilePath(DataPaths::currentProfilePath() + QLatin1String("/adblock/cList.txt"));
        m_subscriptions.append(cList);

        AdBlockSubscription *dList = new AdBlockSubscription(QLatin1String("Adblock YouTube"), this);
        dList->setUrl(QUrl(ADBLOCK_AY_URL));
        dList->setFilePath(DataPaths::currentProfilePath() + QLatin1String("/adblock/dList.txt"));
        m_subscriptions.append(dList);

        AdBlockSubscription *eList = new AdBlockSubscription(QLatin1String("Adblock Wikipedia"), this);
        eList->setUrl(QUrl(ADBLOCK_AW_URL));
        eList->setFilePath(DataPaths::currentProfilePath() + QLatin1String("/adblock/eList.txt"));
        m_subscriptions.append(eList);

        AdBlockSubscription *fList = new AdBlockSubscription(QLatin1String("Adware Filters"), this);
        fList->setUrl(QUrl(ADBLOCK_AF_URL));
        fList->setFilePath(DataPaths::currentProfilePath() + QLatin1String("/adblock/fList.txt"));
        m_subscriptions.append(fList);

        AdBlockSubscription *hList = new AdBlockSubscription(QLatin1String("Anti-Facebook List"), this);
        hList->setUrl(QUrl(ADBLOCK_AFLIST_URL));
        hList->setFilePath(DataPaths::currentProfilePath() + QLatin1String("/adblock/hList.txt"));
        m_subscriptions.append(hList);

        AdBlockSubscription *IList = new AdBlockSubscription(QLatin1String("Distractions and Clickbait Filter"), this);
        IList->setUrl(QUrl(ADBLOCK_DAC_URL));
        IList->setFilePath(DataPaths::currentProfilePath() + QLatin1String("/adblock/IList.txt"));
        m_subscriptions.append(IList);

        AdBlockSubscription *JList = new AdBlockSubscription(QLatin1String("EasyList Cookie List"), this);
        JList->setUrl(QUrl(ADBLOCK_ELCL_URL));
        JList->setFilePath(DataPaths::currentProfilePath() + QLatin1String("/adblock/JList.txt"));
        m_subscriptions.append(JList);
/* // Too aggressive as it blocks sign-in with Google
        AdBlockSubscription *KList = new AdBlockSubscription(QLatin1String("Fanboy's Annoyance List"), this);
        KList->setUrl(QUrl(ADBLOCK_FBAL_URL));
        KList->setFilePath(DataPaths::currentProfilePath() + QLatin1String("/adblock/KList.txt"));
        m_subscriptions.append(KList);
*/
        AdBlockSubscription *RList = new AdBlockSubscription(QLatin1String("I Don't Care about Cookies"), this);
        RList->setUrl(QUrl(ADBLOCK_IDCAC_URL));
        RList->setFilePath(DataPaths::currentProfilePath() + QLatin1String("/adblock/RList.txt"));
        m_subscriptions.append(RList);

        AdBlockSubscription *SList = new AdBlockSubscription(QLatin1String("I don't care about newsletters"), this);
        SList->setUrl(QUrl(ADBLOCK_IDCANL_URL));
        SList->setFilePath(DataPaths::currentProfilePath() + QLatin1String("/adblock/SList.txt"));
        m_subscriptions.append(SList);

        AdBlockSubscription *LList = new AdBlockSubscription(QLatin1String("Linked Insanity Annoyance Rules"), this);
        LList->setUrl(QUrl(ADBLOCK_LIA_URL));
        LList->setFilePath(DataPaths::currentProfilePath() + QLatin1String("/adblock/LList.txt"));
        m_subscriptions.append(LList);

        AdBlockSubscription *MList = new AdBlockSubscription(QLatin1String("Prebake Obtrusive"), this);
        MList->setUrl(QUrl(ADBLOCK_PO_URL));
        MList->setFilePath(DataPaths::currentProfilePath() + QLatin1String("/adblock/MList.txt"));
        m_subscriptions.append(MList);

        AdBlockSubscription *NList = new AdBlockSubscription(QLatin1String("Spam404"), this);
        NList->setUrl(QUrl(ADBLOCK_S404_URL));
        NList->setFilePath(DataPaths::currentProfilePath() + QLatin1String("/adblock/NList.txt"));
        m_subscriptions.append(NList);

        AdBlockSubscription *OList = new AdBlockSubscription(QLatin1String("The Hosts File Project Adblock Filters"), this);
        OList->setUrl(QUrl(ADBLOCK_HFPAF_URL));
        OList->setFilePath(DataPaths::currentProfilePath() + QLatin1String("/adblock/OList.txt"));
        m_subscriptions.append(OList);

        AdBlockSubscription *QList = new AdBlockSubscription(QLatin1String("Twitch: Pure Viewing Experience"), this);
        QList->setUrl(QUrl(ADBLOCK_TPVE_URL));
        QList->setFilePath(DataPaths::currentProfilePath() + QLatin1String("/adblock/QList.txt"));
        m_subscriptions.append(QList);

    }

    // Append CustomList
    AdBlockCustomList* customList = new AdBlockCustomList(this);
    m_subscriptions.prepend(customList);

    // Load all subscriptions
    for (AdBlockSubscription* subscription : qAsConst(m_subscriptions)) {
        subscription->loadSubscription(m_disabledRules);

        connect(subscription, &AdBlockSubscription::subscriptionUpdated, mApp, &MainApplication::reloadUserStyleSheet);
        connect(subscription, &AdBlockSubscription::subscriptionChanged, this, &AdBlockManager::updateMatcher);
    }

    if (lastUpdate.addDays(14) < QDateTime::currentDateTime()) {
        QTimer::singleShot(150000, this, &AdBlockManager::updateAllSubscriptions); // 2.5 Minutes
    }

#ifdef ADBLOCK_DEBUG
    qDebug() << "AdBlock loaded in" << timer.elapsed();
#endif

    m_matcher->update();
    m_loaded = true;

    connect(m_interceptor, &AdBlockUrlInterceptor::requestBlocked, this, [this](const AdBlockedRequest &request) {
        m_blockedRequests[request.firstPartyUrl].append(request);
        emit blockedRequestsChanged(request.firstPartyUrl);
    });

    mApp->networkManager()->installUrlInterceptor(m_interceptor);
}

void AdBlockManager::updateMatcher()
{
    QMutexLocker locker(&m_mutex);

    mApp->networkManager()->removeUrlInterceptor(m_interceptor);
    m_matcher->update();
    mApp->networkManager()->installUrlInterceptor(m_interceptor);
}

void AdBlockManager::updateAllSubscriptions()
{
    for (AdBlockSubscription* subscription : qAsConst(m_subscriptions)) {
        subscription->updateSubscription();
    }

    Settings settings;
    settings.beginGroup(QStringLiteral("AdBlock"));
    settings.setValue(QStringLiteral("lastUpdate"), QDateTime::currentDateTime());
    settings.endGroup();
}

void AdBlockManager::save()
{
    if (!m_loaded) {
        return;
    }

    for (AdBlockSubscription* subscription : qAsConst(m_subscriptions)) {
        subscription->saveSubscription();
    }

    Settings settings;
    settings.beginGroup(QStringLiteral("AdBlock"));
    settings.setValue(QStringLiteral("enabled"), m_enabled);
    settings.setValue(QStringLiteral("disabledRules"), m_disabledRules);
    settings.endGroup();
}

bool AdBlockManager::isEnabled() const
{
    return m_enabled;
}

bool AdBlockManager::canRunOnScheme(const QString &scheme) const
{
    return !(scheme == QLatin1String("file") || scheme == QLatin1String("qrc") || scheme == QLatin1String("view-source")
             || scheme == QLatin1String("browser") || scheme == QLatin1String("data") || scheme == QLatin1String("abp"));
}

bool AdBlockManager::canBeBlocked(const QUrl &url) const
{
    return !m_matcher->adBlockDisabledForUrl(url);
}

QString AdBlockManager::elementHidingRules(const QUrl &url) const
{
    if (!isEnabled() || !canRunOnScheme(url.scheme()) || !canBeBlocked(url))
        return QString();

    return m_matcher->elementHidingRules();
}

QString AdBlockManager::elementHidingRulesForDomain(const QUrl &url) const
{
    if (!isEnabled() || !canRunOnScheme(url.scheme()) || !canBeBlocked(url))
        return QString();

    return m_matcher->elementHidingRulesForDomain(url.host());
}

AdBlockSubscription* AdBlockManager::subscriptionByName(const QString &name) const
{
    for (AdBlockSubscription* subscription : qAsConst(m_subscriptions)) {
        if (subscription->title() == name) {
            return subscription;
        }
    }

    return nullptr;
}

AdBlockDialog *AdBlockManager::showDialog(QWidget *parent)
{
    if (!m_adBlockDialog) {
        m_adBlockDialog = new AdBlockDialog(parent ? parent : mApp->getWindow());
    }

    m_adBlockDialog.data()->show();
    m_adBlockDialog.data()->raise();
    m_adBlockDialog.data()->activateWindow();

    return m_adBlockDialog.data();
}

void AdBlockManager::showRule()
{
    if (QAction* action = qobject_cast<QAction*>(sender())) {
        const AdBlockRule* rule = static_cast<const AdBlockRule*>(action->data().value<void*>());

        if (rule) {
            showDialog()->showRule(rule);
        }
    }
}
