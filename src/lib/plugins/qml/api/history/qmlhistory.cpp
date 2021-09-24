/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2018 Anmol Gautam <tarptaeya@gmail.com>
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
#include "qmlhistory.h"
#include "mainapplication.h"
#include "history.h"
#include "sqldatabase.h"
#include "qml/qmlstaticdata.h"
#include <QQmlEngine>

QmlHistory::QmlHistory(QObject *parent)
    : QObject(parent)
{
    connect(mApp->history(), &History::historyEntryAdded, this, [this](const HistoryEntry &entry){
        QmlHistoryItem *historyItem = QmlStaticData::instance().getHistoryItem(entry);
        emit visited(historyItem);
    });

    connect(mApp->history(), &History::historyEntryDeleted, this, [this](const HistoryEntry &entry){
        QmlHistoryItem *historyItem = QmlStaticData::instance().getHistoryItem(entry);
        emit visitRemoved(historyItem);
    });
}

QList<QObject*> QmlHistory::search(const QString &text)
{
    QList<QObject*> list;
    const QList<HistoryEntry> result = mApp->history()->searchHistoryEntry(text);
    list.reserve(result.size());
    for (const HistoryEntry &entry : result) {
        QmlHistoryItem * item = QmlStaticData::instance().getHistoryItem(entry);
        list.append(item);
    }
    return list;
}

int QmlHistory::getVisits(const QString &url)
{
    HistoryEntry entry = mApp->history()->getHistoryEntry(url);
    return entry.count;
}

void QmlHistory::addUrl(const QVariantMap &map)
{
    if (!map.contains(QStringLiteral("title")) || !map.contains(QStringLiteral("url"))) {
        qWarning() << "Error:" << "wrong arguments passed to" << __FUNCTION__;
        return;
    }
    QString title = map.value(QStringLiteral("title")).toString();
    const QString url = map.value(QStringLiteral("url")).toString();

    title = title.isEmpty() ? url : title;

    mApp->history()->addHistoryEntry(QUrl::fromEncoded(url.toUtf8()), title);
}

void QmlHistory::deleteUrl(const QString &url)
{
    mApp->history()->deleteHistoryEntry(url);
}

void QmlHistory::deleteRange(const QVariantMap &map)
{
    if (!map.contains(QStringLiteral("startTime")) || !map.contains(QStringLiteral("endTime"))) {
        qWarning() << "Error:" << "wrong arguments passed to" << __FUNCTION__;
        return;
    }
    const qlonglong startTime = map.value(QStringLiteral("startTime")).toLongLong();
    const qlonglong endTime = map.value(QStringLiteral("endTime")).toLongLong();

    const QList<int> entries = mApp->history()->indexesFromTimeRange(startTime, endTime);
    for (int index : entries) {
        mApp->history()->deleteHistoryEntry(index);
    }
}

void QmlHistory::deleteAll()
{
    mApp->history()->clearHistory();
}
