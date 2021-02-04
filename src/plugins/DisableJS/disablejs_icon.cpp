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
#include "disablejs_icon.h"
#include "disablejs_manager.h"
#include "disablejs.h"
#include "settings.h"
#include "mainapplication.h" // This provides acces to mApp
#include "browserwindow.h"
#include "webview.h"

#include <QSettings>

DisableJS_Icon::DisableJS_Icon(DisableJS_Manager *manager) : AbstractButtonInterface(manager), m_manager(manager) {

    if (modeCheck(m_settingsPath) == "no" ) {

        Settings settings;
        settings.beginGroup("Web-Browser-Settings");
        const bool JsStatus = settings.value("allowJavaScript", true).toBool();
        settings.endGroup();

        if (!JsStatus) {
            setIcon(QIcon(":disablejs/data/icon-green.svg"));
            setToolTip(tr("Enable Javascript"));
        } else {
            setIcon(QIcon(":disablejs/data/icon-red.svg"));
            setToolTip(tr("Disable Javascript"));
        }

        disconnect(this, &AbstractButtonInterface::webViewChanged, this, &DisableJS_Icon::webViewChanged);

    } else {

        connect(this, &AbstractButtonInterface::webViewChanged, this, &DisableJS_Icon::webViewChanged);

        setIcon(QIcon(":disablejs/data/icon-green.svg"));
        setToolTip(tr("Enable Javascript"));

        // refresh all pages, check if they are on the approval list
        // set icon colour + tooltip accordingly

        // BrowserWindow *window = mApp->getWindow();
        // window->reload();

    }

    setTitle(tr("DisableJS"));

    connect(this, &AbstractButtonInterface::clicked, this, &DisableJS_Icon::toggleJs);

}

void DisableJS_Icon::updateState() {

    WebView *view = webView();

    if (!view) {

        return;
    }

    const QString curPageHostUrl = view->url().host();
    bool isFoundInList = false;

    struct ApprovedSites {
        QString siteDomain;
    };
    QList<ApprovedSites> domains;

    QSettings settings(m_settingsPath + "/plug-ins.ini", QSettings::IniFormat);

    int size = settings.beginReadArray("domains");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        ApprovedSites sites;
        sites.siteDomain = settings.value("siteDomain").toString();
        if (curPageHostUrl == sites.siteDomain) {
            isFoundInList = true;
        }
    }
    settings.endArray();

    if (isFoundInList) {
        setIcon(QIcon(":disablejs/data/icon-red.svg"));
        setToolTip(tr("Approved"));
    } else {
        setIcon(QIcon(":disablejs/data/icon-green.svg"));
        setToolTip(tr("Not approved"));
    }

}

void DisableJS_Icon::toggleAddRemove() {

    WebView *view = webView();

    if (!view) {

        return;
    }

    const QString curPageHostUrl = view->url().host();
    bool isFoundInList = false;

    struct ApprovedSites {
        QString siteDomain;
    };
    QList<ApprovedSites> domains;

    QSettings settings(m_settingsPath + "/plug-ins.ini", QSettings::IniFormat);

    int size = settings.beginReadArray("domains");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        ApprovedSites sites;
        sites.siteDomain = settings.value("siteDomain").toString();
        domains.append(sites);
        if (curPageHostUrl == sites.siteDomain) {
            isFoundInList = true;
        }
    }
    settings.endArray();

    if (isFoundInList) { // need to empty domains array too???

        // Remove domain (curPageHostUrl) from the list. The icon was clicked and the domain should be removed from the list
        // Disable js for the page (and optionally reload it?) - change icon

        // This part does not work..



    } else {

        // Add the domain (curPageHostUrl) to the list
        // Enable js for the page (and optionally reload it?) - change icon

        int lastIndexEntry = domains.size() + 1;

        settings.beginWriteArray("domains");
        settings.setArrayIndex(lastIndexEntry);
        settings.setValue("siteDomain", curPageHostUrl);
        settings.endArray();

    }

}

void DisableJS_Icon::webViewChanged(WebView *view) {

    updateState();

    if (m_view) {
        disconnect(m_view.data(), &WebView::urlChanged, this, &DisableJS_Icon::updateState);
    }

    m_view = view;

    if (m_view) {
        connect(m_view.data(), &WebView::urlChanged, this, &DisableJS_Icon::updateState);
    }

}

QString DisableJS_Icon::modeCheck(QString sP) {

    QString settingApprovalMode;
    QSettings settings(sP + "/plug-ins.ini", QSettings::IniFormat);
    settings.beginGroup("DisableJS");
    settingApprovalMode = settings.value("approval/mode", "no").toString().toLower();
    settings.endGroup();

    return settingApprovalMode;

}

QString DisableJS_Icon::id() const {

    return QStringLiteral("disablejs-icon");

}

QString DisableJS_Icon::name() const {

    return tr("DisableJS Icon");

}

void DisableJS_Icon::toggleJs() {

    if (modeCheck(m_settingsPath) == "no" ) {

        Settings settings;
        settings.beginGroup("Web-Browser-Settings");
        const bool JsStatus = settings.value("allowJavaScript", true).toBool();

        if (!JsStatus) {
            settings.setValue("allowJavaScript", true);
            setIcon(QIcon(":disablejs/data/icon-red.svg"));
            setToolTip(tr("Disable Javascript"));
        } else {
            settings.setValue("allowJavaScript", false);
            setIcon(QIcon(":disablejs/data/icon-green.svg"));
            setToolTip(tr("Enable Javascript"));
        }

        settings.endGroup();

        mApp->reloadSettings();
        BrowserWindow *window = mApp->getWindow();
        window->reload();

    } else {

        toggleAddRemove();
/*
        //QString test1 = m_view->url().toString();

        //QString test1 = m_view->url().host();
        QString test2 = "test";
        QString test2 = "http://bbc.co.uk";

        if (test1 == test2) {
            setIcon(QIcon(":disablejs/data/icon-green.svg"));
            setToolTip(tr("Approved"));
        } else {
            setIcon(QIcon(":disablejs/data/icon-red.svg"));
            setToolTip(tr("Not approved"));
        }
*/
        // If the current page is in the approval list, remove it from the list
        // If the current page is in the approval list, change the icon colour and the tooltip to ?
        // If the current page is in the approval list, disable js for the page

        // If the current page is not in the approval list, add it to the list
        // If the current page is not in the approval list, change the icon colour and the tooltip to ?
        // If the current page is not in the approval list, enable js for the page


    }

}
