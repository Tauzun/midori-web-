/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2019 David Rosca <nowrep@gmail.com>
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
#include "ocssupport.h"
#include "pluginproxy.h"
#include "datapaths.h"
#include "networkmanager.h"
#include "desktopfile.h"
#include "desktopnotificationsfactory.h"
#include "mainapplication.h"

#include <QDir>
#include <QBuffer>
#include <QUrlQuery>
#include <QNetworkReply>

#include <KArchive/KZip>

Q_GLOBAL_STATIC(OcsSupport, qz_ocs_support)

static DesktopFile readMetaData(const KArchiveDirectory *directory)
{
    const KArchiveEntry *entry = directory->entry(QStringLiteral("metadata.desktop"));
    if (!entry || !entry->isFile()) {
        qWarning() << "No metadata.desktop found";
        return DesktopFile();
    }
    const QString tempDir = DataPaths::path(DataPaths::Temp);
    static_cast<const KArchiveFile*>(entry)->copyTo(tempDir);
    return DesktopFile(tempDir + QLatin1String("/metadata.desktop"));
}

OcsSupport::OcsSupport(QObject *parent)
    : QObject(parent)
{
}

bool OcsSupport::handleUrl(const QUrl &url)
{
    if (url.host() != QLatin1String("install")) {
        return false;
    }

    QUrl fileUrl;
    QString fileType;
    QString fileName;

    const QList<QPair<QString, QString>> items = QUrlQuery(url).queryItems(QUrl::FullyDecoded);
    for (const QPair<QString, QString> &item : items) {
        if (item.first == QLatin1String("url")) {
            fileUrl = QUrl(item.second);
        } else if (item.first == QLatin1String("type")) {
            fileType = item.second;
        } else if (item.first == QLatin1String("filename")) {
            fileName = item.second;
        }
    }

    if (!fileType.startsWith(QLatin1String("midori_"))) {
        return false;
    }

    if (fileType != QLatin1String("bhawk_themes") && fileType != QLatin1String("bhawk_plug-ins")) {
        qWarning() << "Unsupported type" << fileType;
        return false;
    }

    if (!fileUrl.isValid()) {
        qWarning() << "Invalid url" << fileUrl << url;
        return false;
    }

    qInfo() << "Downloading" << fileUrl;

    QNetworkReply *reply = mApp->networkManager()->get(QNetworkRequest(fileUrl));
    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "Error downloading" << fileUrl << reply->error() << reply->errorString();
            return;
        }
        QBuffer buf;
        buf.setData(reply->readAll());
        KZip zip(&buf);
        if (!zip.open(QIODevice::ReadOnly)) {
            qWarning() << "Failed to open archive";
            return;
        }
        QString notifyMessage;
        if (fileType == QLatin1String("bhawk_themes")) {
            installTheme(zip.directory());
        } else if (fileType == QLatin1String("bhawk_plug-ins")) {
            installExtension(zip.directory());
        }
    });

    return true;
}
// static
OcsSupport *OcsSupport::instance()
{
    return qz_ocs_support();
}

static void showErrorTheme() {
    mApp->desktopNotifications()->showNotification(QObject::tr("Installation failed"), QObject::tr("Failed to install theme"));
}

void OcsSupport::installTheme(const KArchiveDirectory *directory)
{
    if (directory->entries().size() != 1) {
        qWarning() << "Invalid archive format";
        showErrorTheme();
        return;
    }

    const QString name = directory->entries().at(0);
    const KArchiveEntry *entry = directory->entry(name);
    if (!entry || !entry->isDirectory()) {
        qWarning() << "Invalid archive format";
        showErrorTheme();
        return;
    }

    const DesktopFile metaData = readMetaData(static_cast<const KArchiveDirectory*>(entry));

    const QString targetDir = DataPaths::path(DataPaths::Config) + QLatin1String("/themes");
    QDir().mkpath(targetDir);

    if (QFileInfo::exists(targetDir + QLatin1Char('/') + name)) {
        qWarning() << "Theme" << name << "already exists";
        mApp->desktopNotifications()->showNotification(tr("Installation failed"), tr("Theme is already installed"));
        return;
    }

    if (!directory->copyTo(targetDir)) {
        qWarning() << "Failed to copy theme to" << targetDir;
        showErrorTheme();
        return;
    }

    qInfo() << "Theme installed to" << targetDir;

    mApp->desktopNotifications()->showNotification(tr("Theme installed"), tr("'%1' was successfully installed").arg(metaData.name()));
}

static void showErrorExtension() {
    mApp->desktopNotifications()->showNotification(QObject::tr("Installation failed"), QObject::tr("Failed to install plug-in"));
}

void OcsSupport::installExtension(const KArchiveDirectory *directory)
{
    if (directory->entries().size() != 1) {
        qWarning() << "Invalid archive format";
        showErrorExtension();

        return;
    }

    const QString name = directory->entries().at(0);
    const KArchiveEntry *entry = directory->entry(name);
    if (!entry || !entry->isDirectory()) {
        qWarning() << "Invalid archive format";
        showErrorExtension();

        return;
    }

    const DesktopFile metaData = readMetaData(static_cast<const KArchiveDirectory*>(entry));
    const QString extensionType = metaData.value(QStringLiteral("X-Bhawk-Type")).toString();

    QString type;
    if (extensionType == QLatin1String("Plug-in/Python")) {
        type = QStringLiteral("python");
    } else if (extensionType == QLatin1String("Plug-in/Qml")) {
        type = QStringLiteral("qml");
    }

    if (type.isEmpty()) {
        qWarning() << "Unsupported plug-in type" << extensionType;
        showErrorExtension();

        return;
    }

    const QString targetDir = DataPaths::path(DataPaths::Config) + QLatin1String("/plug-ins/");
    QDir().mkpath(targetDir);

    if (QFileInfo::exists(targetDir + QLatin1String("/") + name)) {
        qWarning() << "Plug-in" << name << "already exists";
        mApp->desktopNotifications()->showNotification(tr("Installation failed"), tr("The plug-in is already installed"));
        return;
    }

    if (!directory->copyTo(targetDir)) {
        qWarning() << "Failed to copy plug-in to" << targetDir;
        showErrorExtension();
        return;
    }

    qInfo() << "Plug-in installed to" << targetDir;

    const QString fullId = QStringLiteral("%1:%2/%3").arg(type, targetDir, name);
    if (!mApp->plugins()->addPlugin(fullId)) {
        qWarning() << "Failed to add plugin" << fullId;
        showErrorExtension();
        return;
    }

    mApp->desktopNotifications()->showNotification(tr("Plug-in installed"), tr("'%1' was successfully installed").arg(metaData.name()));
}
