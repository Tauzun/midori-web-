/* ============================================================
* Blue Hawk web browser
* Copyright (C) 2018-∞ TW3
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
#include "disablejs.h"
#include "disablejs_icon.h"
#include "disablejs_manager.h"
#include "browserwindow.h"
#include "navigationbar.h"
#include "statusbar.h"
#include "pluginproxy.h"
#include "mainapplication.h"
#include "webhittestresult.h"
#include "qzsettings.h"
#include "../config.h"

#include <QMenu>
#include <QPushButton>
#include <QRadioButton>
#include <QGroupBox>
#include <QSettings>

DisableJS::DisableJS() : QObject(), m_manager(0) {

}

void DisableJS::init(InitState state, const QString &settingsPath) {

    qDebug() << __FUNCTION__ << "called";

    // This function is called right after plugin is loaded
    // it will be called even if we return false from testPlugin()
    // so it is recommended not to call any bhawk function here

    // Settings path is PROFILE/plug-ins (without trailign slash),
    // in this directory you can use global .ini file for QSettings
    // named "plug-ins.ini" or create new folder for your plugin
    // and save in it anything you want
    m_settingsPath = settingsPath;

    // State can be either StartupInitState or LateInitState, and it
    // indicates when the plugin have been loaded.
    // Currently, it can be from preferences, or automatically at startup.
    // Plugins are loaded before first BrowserWindow is created.
    Q_UNUSED(state)

    // read approval list from file and insert it into m_approvalList here

    m_manager = new DisableJS_Manager(settingsPath, this);

    connect(mApp->plugins(), &PluginProxy::mainWindowCreated, m_manager, &DisableJS_Manager::mainWindowCreated);
    connect(mApp->plugins(), &PluginProxy::mainWindowDeleted, m_manager, &DisableJS_Manager::mainWindowDeleted);

    const QList<BrowserWindow *> windows = mApp->windows();

    for (BrowserWindow *window : windows) {
        m_manager->mainWindowCreated(window);
    }

}

void DisableJS::unload() {

    qDebug() << __FUNCTION__ << "called";

    delete m_settings.data();
    
    m_manager->unloadPlugin();
    delete m_manager;

}

bool DisableJS::testPlugin() {

    return (Qz::VERSION == QLatin1String(MIDORI_VERSION));

}

bool DisableJS::saveSettings(const QString sType, const QString sValue) {

    QSettings settings(m_settingsPath + "/plug-ins.ini", QSettings::IniFormat);
    settings.beginGroup("DisableJS");
    settings.setValue(sType, sValue);
    settings.endGroup();

    return true;

}

bool DisableJS::r1() {

    DisableJS::saveSettings("approvalMode", "no");

    return true;

}

bool DisableJS::r2() {

    DisableJS::saveSettings("approvalMode", "yes");

    return true;

}

bool DisableJS::removeFromList() {

    // remove from QListWidget and list

    return true;

}

bool DisableJS::onListItemClicked(QListWidgetItem* item) {

    // item is the QListWidgetItem which was clicked on

    currentItem = item;

    return true;

}

void DisableJS::showSettings(QWidget* parent) {

    QString settingApprovalMode;
    QSettings settings(m_settingsPath + "/plug-ins.ini", QSettings::IniFormat);

    settings.beginGroup("DisableJS");
    settingApprovalMode = settings.value("approvalMode", "no").toString().toLower();
    settings.endGroup();

    struct ApprovedSites {
        QString siteDomain;
    };
    QList<ApprovedSites> domains;

    int arraySize = settings.beginReadArray("domains");
    for (int i = 1; i < arraySize; ++i) {
        settings.setArrayIndex(i);
        ApprovedSites sites;
        sites.siteDomain = settings.value("siteDomain").toString();
        domains.append(sites);
    }
    settings.endArray();

    if (!m_settings) {
        m_settings = new QDialog(parent);
        m_settings->resize(480, 320);
        QPushButton* closeButton = new QPushButton(tr("Close"));
        QLabel* textInfo = new QLabel();
        textInfo->setText("DisableJS Plugin Settings");
        textInfo->setStyleSheet("font-weight:bold");

        QVBoxLayout* vbox = new QVBoxLayout;
        vbox->setAlignment(Qt::AlignCenter);
        QGroupBox* groupBox = new QGroupBox();
        QRadioButton* radio1 = new QRadioButton(tr("Browser O&n/Off Mode"));
        QRadioButton* radio2 = new QRadioButton(tr("Approval Mode"));
        QLabel* listTitleInfo = new QLabel();
        listTitleInfo->setText("Approval List");
        QListWidget* listWidget = new QListWidget(); // List of approved sites
        QPushButton* removeListItemButton = new QPushButton(tr("Remove"));
        // removeListItemButton->setEnabled(false); // Disable this button when items in the list are not selected

        if (domains.size() > 0) {
            for (int i = 0; i < domains.size(); ++i) {
                QListWidgetItem* lWi = new QListWidgetItem(domains[i].siteDomain, listWidget);
                lWi->setTextAlignment(Qt::AlignCenter);
            }
        }

        if (settingApprovalMode == "yes") {

            radio2->setChecked(true);

        } else {

            radio1->setChecked(true);

        }

        vbox->addWidget(radio1);
        //vbox->addWidget(radio2); // Disabled. WIP for now
        vbox->addStretch(1);
        vbox->addSpacing(10);
        groupBox->setLayout(vbox);

        QVBoxLayout* l = new QVBoxLayout(m_settings.data());
        l->setAlignment(Qt::AlignCenter);

        l->addSpacing(10);
        l->addWidget(textInfo);
        l->addWidget(groupBox);

        if (domains.size() > 0) {
            l->addWidget(listTitleInfo);
            l->addWidget(removeListItemButton);
            l->addWidget(listWidget);
        }

        m_settings.data()->setLayout(l);
        m_settings.data()->setAttribute(Qt::WA_DeleteOnClose);
        m_settings.data()->setWindowTitle(tr("DisableJS Plugin Settings"));
        m_settings.data()->setWindowIcon(QIcon(":icons/menu/javascript.svg"));
        connect(closeButton, SIGNAL(clicked()), m_settings.data(), SLOT(close()));
        connect(removeListItemButton, &QPushButton::clicked, this, &DisableJS::removeFromList);
        connect(radio1, &QRadioButton::clicked, this, &DisableJS::r1);
        connect(radio2, &QRadioButton::clicked, this, &DisableJS::r2);

        connect(listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onListItemClicked(QListWidgetItem*)));

        l->addWidget(closeButton);

    }

    m_settings.data()->show();
    m_settings.data()->raise();

}
