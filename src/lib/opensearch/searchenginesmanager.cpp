/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2017 David Rosca <nowrep@gmail.com>
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
#include "searchenginesmanager.h"
#include "searchenginesdialog.h"
#include "editsearchengine.h"
#include "networkmanager.h"
#include "iconprovider.h"
#include "mainapplication.h"
#include "opensearchreader.h"
#include "opensearchengine.h"
#include "settings.h"
#include "qzsettings.h"
#include "webview.h"
#include "sqldatabase.h"

#include <QNetworkReply>
#include <QMessageBox>
#include <QBuffer>

#include <QUrlQuery>
#include <QSqlQuery>

#define ENSURE_LOADED if (!m_settingsLoaded) loadSettings();

static QIcon iconFromBase64(const QByteArray &data)
{
    QIcon image;
    QByteArray bArray = QByteArray::fromBase64(data);
    QBuffer buffer(&bArray);
    buffer.open(QIODevice::ReadOnly);
    QDataStream in(&buffer);
    in >> image;
    buffer.close();

    if (!image.isNull()) {
        return image;
    }

    return IconProvider::emptyWebIcon();
}

static QByteArray iconToBase64(const QIcon &icon)
{
    QByteArray bArray;
    QBuffer buffer(&bArray);
    buffer.open(QIODevice::WriteOnly);
    QDataStream out(&buffer);
    out << icon;
    buffer.close();
    return bArray.toBase64();
}

SearchEnginesManager::SearchEnginesManager(QObject* parent)
    : QObject(parent)
    , m_settingsLoaded(false)
    , m_saveScheduled(false)
{
    m_startingEngineName = qzSettings->activeEngine;
    m_defaultEngineName = qzSettings->defaultEngine;

    connect(this, &SearchEnginesManager::enginesChanged, this, &SearchEnginesManager::scheduleSave);
}

void SearchEnginesManager::loadSettings()
{
    m_settingsLoaded = true;

    QSqlQuery query(SqlDatabase::instance()->database());
    query.exec("SELECT name, icon, url, shortcut, suggestionsUrl, suggestionsParameters, postData FROM search_engines");

    while (query.next()) {
        Engine en;
        en.name = query.value(0).toString();
        en.icon = iconFromBase64(query.value(1).toByteArray());
        en.url = query.value(2).toString();
        en.shortcut = query.value(3).toString();
        en.suggestionsUrl = query.value(4).toString();
        en.suggestionsParameters = query.value(5).toByteArray();
        en.postData = query.value(6).toByteArray();

        m_allEngines.append(en);

        if (en.name == m_defaultEngineName) {
            m_defaultEngine = en;
        }
    }

    if (m_allEngines.isEmpty()) {
        restoreDefaults();
    }

    if (m_defaultEngine.name.isEmpty()) {
        m_defaultEngine = m_allEngines[0];
    }
}

SearchEngine SearchEnginesManager::engineForShortcut(const QString &shortcut)
{
    Engine returnEngine;

    if (shortcut.isEmpty()) {
        return returnEngine;
    }

    for (const Engine &en : qAsConst(m_allEngines)) {
        if (en.shortcut == shortcut) {
            returnEngine = en;
            break;
        }
    }

    return returnEngine;
}

LoadRequest SearchEnginesManager::searchResult(const Engine &engine, const QString &string)
{
    ENSURE_LOADED;

    // GET search engine
    if (engine.postData.isEmpty()) {
        QByteArray url = engine.url.toUtf8();
        url.replace("%s", QUrl::toPercentEncoding(string));

        return LoadRequest(QUrl::fromEncoded(url));
    }

    // POST search engine
    QByteArray data = engine.postData;
    data.replace("%s", QUrl::toPercentEncoding(string));

    return LoadRequest(QUrl::fromEncoded(engine.url.toUtf8()), LoadRequest::PostOperation, data);
}

LoadRequest SearchEnginesManager::searchResult(const QString &string)
{
    ENSURE_LOADED;

    const Engine en = qzSettings->searchWithDefaultEngine ? m_defaultEngine : m_activeEngine;
    return searchResult(en, string);
}

void SearchEnginesManager::restoreDefaults()
{
    // Have a search engine you think might be suitable here? Please propose a patch

    Engine duck;
    duck.name = "DuckDuckGo";
    duck.icon = QIcon(":/icons/sites/duck.png");
    duck.url = "https://duckduckgo.com/?q=%s&kl=wt-wt&kp=1&kd=-1&kh=1&k5=2&kae=-1&k1=-1&kaj=m&kam=osm";
    duck.shortcut = "d";
    duck.suggestionsUrl = "https://ac.duckduckgo.com/ac/?q=%s&type=list";

    Engine bing;
    bing.name = "Microsoft Bing";
    bing.icon = QIcon(":icons/sites/bing.png");
    bing.url = "https://bing.com/search?q=%s";
    bing.shortcut = "b";
    bing.suggestionsUrl = "https://api.bing.com/osjson.aspx?query=%s";

    Engine google;
    google.name = "Google";
    google.icon = QIcon(":icons/sites/google.png");
    google.url = "https://www.google.com/search?q=%s";
    google.shortcut = "g";
    google.suggestionsUrl = "https://suggestqueries.google.com/complete/search?output=firefox&q=%s";

    addEngine(duck);
    addEngine(bing);
    addEngine(google);

    m_defaultEngineName = qzSettings->defaultEngine;
    m_startingEngineName = qzSettings->activeEngine;

    bool gotDefaultEngine = false;
    QSqlQuery dbEngineQuery(SqlDatabase::instance()->database());
    dbEngineQuery.exec("SELECT name, icon, url, shortcut, suggestionsUrl, suggestionsParameters, postData FROM search_engines");

    while (dbEngineQuery.next()) {
        Engine engineItemObject;
        engineItemObject.name = dbEngineQuery.value(0).toString();
        engineItemObject.icon = iconFromBase64(dbEngineQuery.value(1).toByteArray());
        engineItemObject.url = dbEngineQuery.value(2).toString();
        engineItemObject.shortcut = dbEngineQuery.value(3).toString();
        engineItemObject.suggestionsUrl = dbEngineQuery.value(4).toString();
        engineItemObject.suggestionsParameters = dbEngineQuery.value(5).toByteArray();
        engineItemObject.postData = dbEngineQuery.value(6).toByteArray();

        if (engineItemObject.name == m_defaultEngineName) {
            m_defaultEngine = engineItemObject;
            gotDefaultEngine = true;
        }
    }

    if (gotDefaultEngine == false) { // Should never match this unless there is an sql bug
        if (m_defaultEngineName == duck.name) {
            m_defaultEngine = duck;
        } else if (m_defaultEngineName == bing.name) {
            m_defaultEngine = bing;
        } else if (m_defaultEngineName == google.name) {
            m_defaultEngine = google;
        } else {
            m_defaultEngine = duck;
        }
    }

    emit enginesChanged();

}

// static
QIcon SearchEnginesManager::iconForSearchEngine(const QUrl &url)
{
    QIcon ic = IconProvider::iconForDomain(url);

    if (ic.isNull()) {
        ic = QIcon(QStringLiteral(":icons/menu/search-icon.svg"));
    }

    return ic;
}

void SearchEnginesManager::engineChangedImage()
{
    OpenSearchEngine* engine = qobject_cast<OpenSearchEngine*>(sender());

    if (!engine) {
        return;
    }

    for (Engine e : qAsConst(m_allEngines)) {
        if (e.name == engine->name() &&
            e.url.contains(engine->searchUrl("%s").toString()) &&
            !engine->image().isNull()
           ) {
            int index = m_allEngines.indexOf(e);
            if (index != -1) {
                m_allEngines[index].icon = QIcon(QPixmap::fromImage(engine->image()));

                emit enginesChanged();

                delete engine;
                break;
            }
        }
    }
}

void SearchEnginesManager::editEngine(const Engine &before, const Engine &after)
{
    removeEngine(before);
    addEngine(after);
}

void SearchEnginesManager::addEngine(const Engine &engine)
{
    ENSURE_LOADED;

    if (m_allEngines.contains(engine)) {
        return;
    }

    m_allEngines.append(engine);

    emit enginesChanged();
}

void SearchEnginesManager::addEngineFromForm(const QVariantMap &formData, WebView *view)
{
    if (formData.isEmpty())
        return;

    const QString method = formData.value(QStringLiteral("method")).toString();
    bool isPost = method == QLatin1String("post");

    QUrl actionUrl = formData.value(QStringLiteral("action")).toUrl();

    if (actionUrl.isRelative()) {
        actionUrl = view->url().resolved(actionUrl);
    }

    QUrl parameterUrl = actionUrl;

    if (isPost) {
        parameterUrl = QUrl("https://foo.bar");
    }

    const QString &inputName = formData.value(QStringLiteral("inputName")).toString();

    QUrlQuery query(parameterUrl);
    query.addQueryItem(inputName, QStringLiteral("SEARCH"));

    const QVariantList &inputs = formData.value(QStringLiteral("inputs")).toList();
    for (const QVariant &pair : inputs) {
        const QVariantList &list = pair.toList();
        if (list.size() != 2)
            continue;

        const QString &name = list.at(0).toString();
        const QString &value = list.at(1).toString();

        if (name == inputName || name.isEmpty() || value.isEmpty())
            continue;

        query.addQueryItem(name, value);
    }

    parameterUrl.setQuery(query);

    if (!isPost) {
        actionUrl = parameterUrl;
    }

    SearchEngine engine;
    engine.name = view->title();
    engine.icon = view->icon();
    engine.url = actionUrl.toEncoded();

    if (isPost) {
        QByteArray data = parameterUrl.toEncoded(QUrl::RemoveScheme);
        engine.postData = data.contains('?') ? data.mid(data.lastIndexOf('?') + 1) : QByteArray();
        engine.postData.replace((inputName + QLatin1String("=SEARCH")).toUtf8(), (inputName + QLatin1String("=%s")).toUtf8());
    } else {
        engine.url.replace(inputName + QLatin1String("=SEARCH"), inputName + QLatin1String("=%s"));
    }

    EditSearchEngine dialog(SearchEnginesDialog::tr("Add Search Engine"), view);
    dialog.setName(engine.name);
    dialog.setIcon(engine.icon);
    dialog.setUrl(engine.url);
    dialog.setPostData(engine.postData);

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    engine.name = dialog.name();
    engine.icon = dialog.icon();
    engine.url = dialog.url();
    engine.shortcut = dialog.shortcut();
    engine.postData = dialog.postData().toUtf8();

    if (engine.name.isEmpty() || engine.url.isEmpty()) {
        return;
    }

    addEngine(engine);
}

void SearchEnginesManager::addEngine(OpenSearchEngine* engine)
{
    ENSURE_LOADED;

    Engine en;
    en.name = engine->name();
    en.url = engine->searchUrl("searchstring").toString().replace(QLatin1String("searchstring"), QLatin1String("%s"));

    if (engine->image().isNull()) {
        en.icon = iconForSearchEngine(engine->searchUrl(QString()));
    }
    else {
        en.icon = QIcon(QPixmap::fromImage(engine->image()));
    }

    en.suggestionsUrl = engine->getSuggestionsUrl();
    en.suggestionsParameters = engine->getSuggestionsParameters();
    en.postData = engine->getPostData("searchstring").replace("searchstring", "%s");

    addEngine(en);

    connect(engine, &OpenSearchEngine::imageChanged, this, &SearchEnginesManager::engineChangedImage);
}

void SearchEnginesManager::addEngine(const QUrl &url)
{
    ENSURE_LOADED;

    if (!url.isValid()) {
        return;
    }

    qApp->setOverrideCursor(Qt::WaitCursor);

    QNetworkReply* reply = mApp->networkManager()->get(QNetworkRequest(url));
    reply->setParent(this);
    connect(reply, &QNetworkReply::finished, this, &SearchEnginesManager::replyFinished);
}

void SearchEnginesManager::replyFinished()
{
    qApp->restoreOverrideCursor();

    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        return;
    }

    if (reply->error() != QNetworkReply::NoError) {
        reply->close();
        reply->deleteLater();
        return;
    }

    OpenSearchReader reader;
    OpenSearchEngine* engine = reader.read(reply);
    engine->setNetworkAccessManager(mApp->networkManager());

    reply->close();
    reply->deleteLater();

    if (checkEngine(engine)) {
        addEngine(engine);
        QMessageBox::information(0, tr("Search Engine Added"), tr("Search Engine \"%1\" has been successfully added.").arg(engine->name()));
    }
}

bool SearchEnginesManager::checkEngine(OpenSearchEngine* engine)
{
    if (!engine->isValid()) {
        QString errorString = tr("Search Engine is not valid!");
        QMessageBox::warning(0, tr("Error"), tr("Error while adding Search Engine <br><b>Error Message: </b> %1").arg(errorString));

        return false;
    }

    return true;
}

void SearchEnginesManager::setActiveEngine(const Engine &engine)
{
    ENSURE_LOADED;

    if (!m_allEngines.contains(engine)) {
        return;
    }

    m_activeEngine = engine;

    Settings settings;
    settings.setValue(QStringLiteral("SearchEngines/activeEngine"), m_activeEngine.name);
    qzSettings->activeEngine = QString(m_activeEngine.name);

    emit activeEngineChanged();
}

void SearchEnginesManager::setDefaultEngine(const SearchEnginesManager::Engine &engine)
{
    ENSURE_LOADED;

    if (!m_allEngines.contains(engine)) {
        return;
    }

    m_defaultEngine = engine;

    Settings settings;
    settings.setValue(QStringLiteral("SearchEngines/DefaultEngine"), m_defaultEngine.name);
    qzSettings->defaultEngine = QString(m_defaultEngine.name);

    emit defaultEngineChanged();
}

void SearchEnginesManager::removeEngine(const Engine &engine)
{
    ENSURE_LOADED;

    int index = m_allEngines.indexOf(engine);

    if (index < 0) {
        return;
    }

    QSqlQuery query(SqlDatabase::instance()->database());
    query.prepare("DELETE FROM search_engines WHERE name=? AND url=?");
    query.bindValue(0, engine.name);
    query.bindValue(1, engine.url);
    query.exec();

    m_allEngines.remove(index);
    emit enginesChanged();
}

void SearchEnginesManager::setAllEngines(const QVector<Engine> &engines)
{
    ENSURE_LOADED;

    m_allEngines = engines;
    emit enginesChanged();
}

QVector<SearchEngine> SearchEnginesManager::allEngines()
{
    ENSURE_LOADED;

    return m_allEngines;
}

void SearchEnginesManager::saveSettings()
{

    if (!m_saveScheduled) {
        return;
    }

    // Well, this is not the best implementation to do as this is taking some time.
    // Actually, it is delaying the quit of app for about a 1 sec on my machine with only
    // 5 engines. Another problem is that deleting rows without VACUUM isn't actually freeing
    // space in database.
    //
    // But as long as user is not playing with search engines every run it is acceptable.

    QSqlQuery dbQuery(SqlDatabase::instance()->database());
    dbQuery.exec("DELETE FROM search_engines");

    for (const Engine &en : qAsConst(m_allEngines)) {
        dbQuery.prepare("INSERT INTO search_engines (name, icon, url, shortcut, suggestionsUrl, suggestionsParameters, postData) VALUES (?, ?, ?, ?, ?, ?, ?)");
        dbQuery.addBindValue(en.name);
        dbQuery.addBindValue(iconToBase64(en.icon));
        dbQuery.addBindValue(en.url);
        dbQuery.addBindValue(en.shortcut);
        dbQuery.addBindValue(en.suggestionsUrl);
        dbQuery.addBindValue(en.suggestionsParameters);
        dbQuery.addBindValue(en.postData);

        dbQuery.exec();
    }
}
