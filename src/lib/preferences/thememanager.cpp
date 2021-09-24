/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2014  David Rosca <nowrep@gmail.com>
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
#include "thememanager.h"
#include "ui_thememanager.h"
#include "qztools.h"
#include "settings.h"
#include "datapaths.h"
#include "licenseviewer.h"
#include "preferences.h"
#include "desktopfile.h"
#include "mainapplication.h"

#include <QDir>
#include <QMessageBox>
#include <QtCoreVersion>

ThemeManager::ThemeManager(QWidget* parent, Preferences* preferences)
    : QWidget(parent)
    , ui(new Ui::ThemeManager)
    , m_preferences(preferences)
{
    ui->setupUi(parent);
    ui->listWidget->setLayoutDirection(Qt::LeftToRight);
    ui->license->hide();

    Settings settings;
    m_activeTheme = settings.value("Themes/activeTheme", DEFAULT_THEME_NAME).toString();

    const QStringList themePaths = DataPaths::allPaths(DataPaths::Themes);

    for (const QString &path : themePaths) {
        QDir dir(path);
        const QStringList list = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
        for (const QString &name : list) {
            Theme themeInfo = parseTheme(dir.absoluteFilePath(name) + QLatin1Char('/'), name);
            if (!themeInfo.isValid) {
                continue;
            }

            QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
            item->setText(themeInfo.name);
            item->setIcon(themeInfo.icon);
            item->setData(Qt::UserRole, name);

            if (m_activeTheme == name) {
                ui->listWidget->setCurrentItem(item);
            }

            ui->listWidget->addItem(item);
        }
    }

    ui->themeDirs->setText("<b>Themes directory:</b> " + DataPaths::path(DataPaths::Themes));

#if !defined(Q_OS_WIN)

    const QStringList themeExtraPaths = DataPaths::allPaths(DataPaths::ThemesExtra);

    for (const QString &path : themeExtraPaths) {
        QDir dir(path);
        const QStringList list = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
        for (const QString &name : list) {
            Theme themeInfo = parseTheme(dir.absoluteFilePath(name) + QLatin1Char('/'), name);
            if (!themeInfo.isValid) {
                continue;
            }

            QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
            item->setText(themeInfo.name);
            item->setIcon(themeInfo.icon);
            item->setData(Qt::UserRole, name);

            if (m_activeTheme == name) {
                ui->listWidget->setCurrentItem(item);
            }

            ui->listWidget->addItem(item);
        }
    }

#endif

    QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
    item->setText(QLatin1String("None"));
    item->setIcon(QIcon(QStringLiteral(":/icons/menu/sub-menu.svg")));
    item->setData(Qt::UserRole, QLatin1String("None"));

    Theme info;
    info.name = QLatin1String("None");
    info.description = QLatin1String("No styles at all");
    info.author = QLatin1String("TW3");
    m_themeHash.insert("None", info);

    if (m_activeTheme == QLatin1String("None")) {
        ui->listWidget->setCurrentItem(item);
    }

    ui->listWidget->addItem(item);

    connect(ui->listWidget, &QListWidget::currentItemChanged, this, &ThemeManager::currentChanged);
    connect(ui->license, &ClickableLabel::clicked, this, &ThemeManager::showLicense);
    connect(ui->remove, &QPushButton::clicked, this, &ThemeManager::removeTheme);

    currentChanged();
}

void ThemeManager::showLicense()
{
    QListWidgetItem* currentItem = ui->listWidget->currentItem();
    if (!currentItem) {
        return;
    }

    Theme currentTheme = m_themeHash[currentItem->data(Qt::UserRole).toString()];

    LicenseViewer* v = new LicenseViewer(m_preferences);
    v->setText(currentTheme.license);
    v->show();
}

void ThemeManager::removeTheme()
{
    QListWidgetItem* currentItem = ui->listWidget->currentItem();
    if (!currentItem) {
        return;
    }
    Theme currentTheme = m_themeHash[currentItem->data(Qt::UserRole).toString()];

    const QMessageBox::StandardButton button = QMessageBox::warning(this, tr("Confirmation"),
                                             tr("Are you sure you want to remove '%1'?").arg(currentTheme.name),
                                             QMessageBox::Yes | QMessageBox::No);
    if (button != QMessageBox::Yes) {
        return;
    }

    QDir(currentTheme.themePath).removeRecursively();
    delete currentItem;
}

void ThemeManager::currentChanged()
{
    QListWidgetItem* currentItem = ui->listWidget->currentItem();
    if (!currentItem) {
        return;
    }

    Theme currentTheme = m_themeHash[currentItem->data(Qt::UserRole).toString()];

    ui->name->setText(currentTheme.name);
    ui->author->setText(currentTheme.author);
    ui->description->setText(currentTheme.description);
    ui->license->setHidden(currentTheme.license.isEmpty());

    const int a = QString::compare(currentTheme.name, "Noir", Qt::CaseInsensitive);
    const int b = QString::compare(currentTheme.name, "Vivid", Qt::CaseInsensitive);
    const int c = QString::compare(currentTheme.name, "None", Qt::CaseInsensitive);
    int d = 0;

    if (a == 0) {
        d = 1;
    } else if (b == 0) {
        d = 1;
    } else if (c == 0) {
        d = 1;
    }

    ui->remove->setEnabled(false);

    if (d != 1) {
        ui->remove->setEnabled(QFileInfo(currentTheme.themePath).isWritable());
    }

}

ThemeManager::Theme ThemeManager::parseTheme(const QString &path, const QString &name)
{
    Theme info;
    info.isValid = false;

    if (!QFile(path + "main.css").exists() || !QFile(path + "metadata.desktop").exists()) {
        info.isValid = false;
        return info;
    }

    DesktopFile metadata(path + QStringLiteral("metadata.desktop"));
    info.name = metadata.name();
    info.description = metadata.comment();
    info.author = metadata.value(QStringLiteral("X-Bhawk-Author")).toString();
    info.themePath = path.chopped(1);

    const QString iconName = metadata.icon();
    if (!iconName.isEmpty()) {
        if (QFileInfo::exists(path + iconName)) {
            info.icon = QIcon(path + iconName);
        } else {
            info.icon = QIcon::fromTheme(iconName);
        }
    }

    const QString licensePath = metadata.value(QStringLiteral("X-Bhawk-License")).toString();
    if (!licensePath.isEmpty() && QFileInfo::exists(path + licensePath)) {
        info.license = QzTools::readAllFileContents(path + licensePath);
    }

    if (info.name.isEmpty() || m_themeHash.contains(name)) {
        return info;
    }

    info.isValid = true;
    m_themeHash.insert(name, info);
    return info;
}

void ThemeManager::save()
{
    QListWidgetItem* currentItem = ui->listWidget->currentItem();
    if (!currentItem) {
        return;
    }

    Settings settings;
    settings.setValue("Themes/activeTheme", currentItem->data(Qt::UserRole));

}

ThemeManager::~ThemeManager()
{
    delete ui;
}
