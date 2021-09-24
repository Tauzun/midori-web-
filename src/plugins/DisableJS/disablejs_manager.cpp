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
#include "disablejs_manager.h"
#include "disablejs_icon.h"
#include "disablejs.h"
#include "browserwindow.h"
#include "qztools.h"
#include "mainapplication.h"
#include "navigationbar.h"
#include "statusbar.h"
#include "settings.h"

#include <QSettings>

DisableJS_Manager::DisableJS_Manager(const QString &sPath, QObject* parent) : QObject(parent), m_settingsPath(sPath) {

    load();

}

void DisableJS_Manager::unloadPlugin() {
    // Save settings
    QSettings settings(m_settingsPath + "/plug-ins.ini", QSettings::IniFormat);
    settings.beginGroup("DisableJS");
    // There is nothing here yet
    settings.endGroup();
    // Remove icons from all windows
    QHashIterator<BrowserWindow*, DisableJS_Icon*> it(m_windows);

    while (it.hasNext()) {
        it.next();
        mainWindowDeleted(it.key());
    }

}

void DisableJS_Manager::load() {

    QDir djsDir(m_settingsPath + QLatin1String("/disablejs"));

    if (!djsDir.exists()) {
        djsDir.mkdir(m_settingsPath + QLatin1String("/disablejs"));
    }

}

void DisableJS_Manager::mainWindowCreated(BrowserWindow* window) {

    DisableJS_Icon *icon = new DisableJS_Icon(this);
    m_windows[window] = icon;
    window->statusBar()->addButton(icon);
    window->navigationBar()->addToolButton(icon);

}

void DisableJS_Manager::mainWindowDeleted(BrowserWindow* window) {

    DisableJS_Icon *icon = m_windows.take(window);
    window->statusBar()->removeButton(icon);
    window->navigationBar()->removeToolButton(icon);
    delete icon;

    // re-enable js here ?

}
