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
#ifndef ADBLOCKMANAGER_H
#define ADBLOCKMANAGER_H

#include <QObject>
#include <QStringList>
#include <QPointer>
#include <QMutex>
#include <QUrl>
#include <QWebEngineUrlRequestInfo>

#include "qzcommon.h"

#define ADBLOCK_ELIST_URL QStringLiteral("https://easylist.to/easylist/easylist.txt")
#define ADBLOCK_PLIST_URL QStringLiteral("https://pgl.yoyo.org/adservers/serverlist.php?hostformat=adblockplus&mimetype=plaintext")
#define ADBLOCK_ABPACFL_URL QStringLiteral("https://easylist-downloads.adblockplus.org/abp-filters-anti-cv.txt")
#define ADBLOCK_AY_URL QStringLiteral("https://cdn.jsdelivr.net/gh/kbinani/adblock-youtube-ads/signed.txt")
#define ADBLOCK_AW_URL QStringLiteral("https://cdn.jsdelivr.net/gh/kbinani/adblock-wikipedia/signed.txt")
#define ADBLOCK_AF_URL QStringLiteral("https://easylist-downloads.adblockplus.org/adwarefilters.txt")
#define ADBLOCK_APAR_URL QStringLiteral("https://anonymousposter.gitlab.io/ublockorigin-rules/blockrules.txt")
#define ADBLOCK_AFLIST_URL QStringLiteral("https://fanboy.co.nz/fanboy-antifacebook.txt")
#define ADBLOCK_DAC_URL QStringLiteral("https://cdn.jsdelivr.net/gh/endolith/clickbait/clickbait.txt")
#define ADBLOCK_ELCL_URL QStringLiteral("https://easylist-downloads.adblockplus.org/easylist-cookie.txt")
#define ADBLOCK_FBAL_URL QStringLiteral("https://easylist-downloads.adblockplus.org/fanboy-annoyance.txt")
#define ADBLOCK_IDCAC_URL QStringLiteral("https://www.i-dont-care-about-cookies.eu/abp/")
#define ADBLOCK_IDCANL_URL QStringLiteral("https://cdn.jsdelivr.net/gh/Manu1400/i-don-t-care-about-newsletters/adp.txt")
#define ADBLOCK_LIA_URL QStringLiteral("https://cdn.jsdelivr.net/gh/taylr/linkedinsanity/linkedinsanity.txt")
#define ADBLOCK_PO_URL QStringLiteral("https://cdn.jsdelivr.net/gh/liamja/Prebake/obtrusive.txt")
#define ADBLOCK_S404_URL QStringLiteral("https://cdn.jsdelivr.net/gh/Spam404/lists/adblock-list.txt")
#define ADBLOCK_HFPAF_URL QStringLiteral("https://hostsfile.mine.nu/downloads/adblock.txt")
#define ADBLOCK_TCAF_URL QStringLiteral("https://cdn.jsdelivr.net/gh/thoughtconverge/abf/abf.txt")
#define ADBLOCK_TPVE_URL QStringLiteral("https://cdn.jsdelivr.net/gh/DandelionSprout/adfilt/TwitchPureViewingExperience.txt")

class AdBlockRule;
class AdBlockDialog;
class AdBlockMatcher;
class AdBlockCustomList;
class AdBlockSubscription;
class AdBlockUrlInterceptor;

struct AdBlockedRequest
{
    QUrl requestUrl;
    QUrl firstPartyUrl;
    QByteArray requestMethod;
    QWebEngineUrlRequestInfo::ResourceType resourceType;
    QWebEngineUrlRequestInfo::NavigationType navigationType;
    QString rule;
};
Q_DECLARE_METATYPE(AdBlockedRequest)

class MIDORI_EXPORT AdBlockManager : public QObject
{
    Q_OBJECT

public:
    AdBlockManager(QObject* parent = 0);
    ~AdBlockManager();

    void load();
    void save();

    bool isEnabled() const;
    bool canRunOnScheme(const QString &scheme) const;
    bool canBeBlocked(const QUrl &url) const;

    QString elementHidingRules(const QUrl &url) const;
    QString elementHidingRulesForDomain(const QUrl &url) const;

    AdBlockSubscription* subscriptionByName(const QString &name) const;
    QList<AdBlockSubscription*> subscriptions() const;

    bool block(QWebEngineUrlRequestInfo &request, QString &ruleFilter, QString &ruleSubscription);

    QVector<AdBlockedRequest> blockedRequestsForUrl(const QUrl &url) const;
    void clearBlockedRequestsForUrl(const QUrl &url);

    QStringList disabledRules() const;
    void addDisabledRule(const QString &filter);
    void removeDisabledRule(const QString &filter);

    bool addSubscriptionFromUrl(const QUrl &url);

    AdBlockSubscription* addSubscription(const QString &title, const QString &url);
    bool removeSubscription(AdBlockSubscription* subscription);

    AdBlockCustomList* customList() const;

    static AdBlockManager* instance();

Q_SIGNALS:
    void enabledChanged(bool enabled);
    void blockedRequestsChanged(const QUrl &url);

public Q_SLOTS:
    void setEnabled(bool enabled);
    void showRule();

    void updateMatcher();
    void updateAllSubscriptions();

    AdBlockDialog *showDialog(QWidget *parent = nullptr);

private:
    bool m_loaded;
    bool m_enabled;

    QList<AdBlockSubscription*> m_subscriptions;
    AdBlockMatcher* m_matcher;
    QStringList m_disabledRules;

    AdBlockUrlInterceptor *m_interceptor;
    QPointer<AdBlockDialog> m_adBlockDialog;
    QMutex m_mutex;
    QHash<QUrl, QVector<AdBlockedRequest>> m_blockedRequests;
};

#endif // ADBLOCKMANAGER_H

