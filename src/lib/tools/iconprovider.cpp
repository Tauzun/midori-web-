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
#include "iconprovider.h"
#include "mainapplication.h"
#include "networkmanager.h"
#include "sqldatabase.h"
#include "autosaver.h"
#include "webview.h"
#include "qztools.h"

#include <QTimer>
#include <QBuffer>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>

Q_GLOBAL_STATIC(IconProvider, qz_icon_provider)

static QByteArray encodeUrl(const QUrl &url)
{
    return url.toEncoded(QUrl::RemoveFragment | QUrl::StripTrailingSlash);
}

IconProvider::IconProvider()
    : QWidget()
{
    m_autoSaver = new AutoSaver(this);
    connect(m_autoSaver, &AutoSaver::save, this, &IconProvider::saveIconsToDatabase);
}

void IconProvider::saveIcon(WebView* view)
{
    // Don't save icons in private mode.
    if (mApp->isPrivate()) {
        return;
    }

    const QIcon icon = view->icon(true);
    if (icon.isNull()) {
        return;
    }

    const QStringList ignoredSchemes = {
        QStringLiteral("browser"),
        QStringLiteral("ftp"),
        QStringLiteral("file"),
        QStringLiteral("view-source"),
        QStringLiteral("data"),
        QStringLiteral("about")
    };

    if (ignoredSchemes.contains(view->url().scheme())) {
        return;
    }

    for (int i = 0; i < m_iconBuffer.size(); ++i) {
        if (m_iconBuffer[i].first == view->url()) {
            m_iconBuffer.removeAt(i);
            break;
        }
    }

    BufferedIcon item;
    item.first = view->url();
    item.second = icon.pixmap(16).toImage();

    m_autoSaver->changeOccurred();
    m_iconBuffer.append(item);
}

QIcon IconProvider::bookmarkIcon() const
{
    return QIcon(QStringLiteral(":icons/menu/bookmarks.svg"));
}

void IconProvider::setBookmarkIcon(const QIcon &icon)
{
    m_bookmarkIcon = icon;
}

QIcon IconProvider::standardIcon(QStyle::StandardPixmap icon)
{
    switch (icon) {
    case QStyle::SP_MessageBoxCritical:
        return QIcon(QStringLiteral(":icons/menu/issue.svg"));

    case QStyle::SP_MessageBoxInformation:
        return QIcon(QStringLiteral(":icons/menu/dialog-information.svg"));

    case QStyle::SP_MessageBoxQuestion:
        return QIcon(QStringLiteral(":icons/menu/help-contents.svg"));

    case QStyle::SP_MessageBoxWarning:
        return QIcon(QStringLiteral(":icons/menu/document-not-encrypted.svg"));

    case QStyle::SP_DialogCloseButton:
        return QIcon(QStringLiteral(":icons/menu/application-exit.svg"));

    case QStyle::SP_BrowserStop:
        return QIcon(QStringLiteral(":icons/menu/process-stop.svg"));

    case QStyle::SP_BrowserReload:
        return QIcon(QStringLiteral(":icons/menu/process-restore.svg"));

    case QStyle::SP_FileDialogToParent:
        return QIcon(QStringLiteral(":icons/menu/document-open-remote.svg"));

    case QStyle::SP_ArrowUp:
        return QIcon(QStringLiteral(":icons/menu/go-up.svg"));

    case QStyle::SP_ArrowDown:
        return QIcon(QStringLiteral(":icons/menu/go-down.svg"));

    case QStyle::SP_ArrowForward:
        if (QApplication::layoutDirection() == Qt::RightToLeft) {
            return QIcon(QStringLiteral(":/icons/menu/go-previous.svg"));
        }
        return QIcon(QStringLiteral(":/icons/menu/go-next.svg"));

    case QStyle::SP_ArrowBack:
        if (QApplication::layoutDirection() == Qt::RightToLeft) {
            return QIcon(QStringLiteral(":/icons/menu/go-next.svg"));
        }
        return QIcon(QStringLiteral(":/icons/menu/go-previous.svg"));

    default:
        return QApplication::style()->standardIcon(icon);
    }
}

QIcon IconProvider::newTabIcon()
{
    return QIcon(QStringLiteral(":/icons/menu/tab-new.svg"));
}

QIcon IconProvider::newWindowIcon()
{
    return QIcon(QStringLiteral(":/icons/menu/window-new.svg"));
}

QIcon IconProvider::privateBrowsingIcon()
{
    return QIcon(QStringLiteral(":/icons/menu/window-private.svg"));
}

QIcon IconProvider::avatarIcon()
{
    return QIcon(QStringLiteral(":/icons/menu/avatar.svg"));
}

QIcon IconProvider::sessionManagerIcon()
{
    return QIcon(QStringLiteral(":/icons/menu/session-manager.svg"));
}

QIcon IconProvider::settingsIcon()
{
    return QIcon(QStringLiteral(":/icons/menu/settings.svg"));
}

QIcon IconProvider::emptyWebIcon()
{
    return QPixmap::fromImage(instance()->emptyWebImage());
}

QImage IconProvider::emptyWebImage()
{
    if (instance()->m_emptyWebImage.isNull()) {
        instance()->m_emptyWebImage = QIcon(QStringLiteral(":icons/other/webpage.svg")).pixmap(16).toImage();
    }

    return instance()->m_emptyWebImage;
}

QIcon IconProvider::iconForUrl(const QUrl &url, bool allowNull)
{
    return instance()->iconFromImage(imageForUrl(url, allowNull));
}

QImage IconProvider::imageForUrl(const QUrl &url, bool allowNull)
{
    if (url.path().isEmpty()) {
        return allowNull ? QImage() : IconProvider::emptyWebImage();
    }

    QMutexLocker locker(&instance()->m_iconCacheMutex);

    const QByteArray encodedUrl = encodeUrl(url);

    if (QImage *img = instance()->m_urlImageCache.object(encodedUrl)) {
        return img->isNull() && !allowNull ? IconProvider::emptyWebImage() : *img;
    }

    const auto iconBuffer = instance()->m_iconBuffer;
    for (const BufferedIcon &ic : iconBuffer) {
        if (encodeUrl(ic.first) == encodedUrl) {
            return ic.second;
        }
    }

    QSqlQuery query(SqlDatabase::instance()->database());
    query.prepare(QStringLiteral("SELECT icon FROM icons WHERE url GLOB ? LIMIT 1"));
    query.addBindValue(QString("%1*").arg(QzTools::escapeSqlGlobString(QString::fromUtf8(encodedUrl))));
    query.exec();

    QImage *img = new QImage;
    if (query.next()) {
        img->loadFromData(query.value(0).toByteArray());
    }
    instance()->m_urlImageCache.insert(encodedUrl, img);

    return img->isNull() && !allowNull ? IconProvider::emptyWebImage() : *img;
}

QIcon IconProvider::iconForDomain(const QUrl &url, bool allowNull)
{
    return instance()->iconFromImage(imageForDomain(url, allowNull));
}

QImage IconProvider::imageForDomain(const QUrl &url, bool allowNull)
{
    if (url.host().isEmpty()) {
        return allowNull ? QImage() : IconProvider::emptyWebImage();
    }

    QMutexLocker locker(&instance()->m_iconCacheMutex);

    const auto iconBuffer = instance()->m_iconBuffer;
    for (const BufferedIcon &ic : iconBuffer) {
        if (ic.first.host() == url.host()) {
            return ic.second;
        }
    }

    QSqlQuery query(SqlDatabase::instance()->database());
    query.prepare(QStringLiteral("SELECT icon FROM icons WHERE url GLOB ? LIMIT 1"));
    query.addBindValue(QString("*%1*").arg(QzTools::escapeSqlGlobString(url.host())));
    query.exec();

    if (query.next()) {
        return QImage::fromData(query.value(0).toByteArray());
    }

    return allowNull ? QImage() : IconProvider::emptyWebImage();
}

IconProvider* IconProvider::instance()
{
    return qz_icon_provider();
}

void IconProvider::saveIconsToDatabase()
{
    QMutexLocker locker(&instance()->m_iconCacheMutex);

    for (const BufferedIcon &ic : qAsConst(m_iconBuffer)) {
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        ic.second.save(&buffer, "PNG");

        const QByteArray encodedUrl = encodeUrl(ic.first);
        m_urlImageCache.remove(encodedUrl);

        auto job = new SqlQueryJob(QStringLiteral("INSERT OR REPLACE INTO icons (icon, url) VALUES (?,?)"), this);
        job->addBindValue(buffer.data());
        job->addBindValue(QString::fromUtf8(encodedUrl));
        job->start();
    }

    m_iconBuffer.clear();
}

void IconProvider::clearOldIconsInDatabase()
{
    // Delete icons for entries older than 6 months
    const QDateTime date = QDateTime::currentDateTime().addMonths(-6);

    QSqlQuery query(SqlDatabase::instance()->database());
    query.prepare(QStringLiteral("DELETE FROM icons WHERE url IN (SELECT url FROM history WHERE date < ?)"));
    query.addBindValue(date.toMSecsSinceEpoch());
    query.exec();

    query.clear();
    query.exec(QStringLiteral("VACUUM"));
}

QIcon IconProvider::iconFromImage(const QImage &image)
{
    return QIcon(QPixmap::fromImage(image));
}
