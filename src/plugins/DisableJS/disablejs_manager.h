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
#ifndef DISABLEJS_MANAGER_H
#define DISABLEJS_MANAGER_H

#include "plugininterface.h"

#include <QObject>
#include <QStringList>
#include <QHash>

class BrowserWindow;
class DisableJS_Icon;

class DisableJS_Manager : public QObject
{
    Q_OBJECT
public:
    explicit DisableJS_Manager(const QString &sPath, QObject* parent = 0);
    void unloadPlugin();

    QHash<QString, int> m_approvalList;

public Q_SLOTS:
    void mainWindowCreated(BrowserWindow* window);
    void mainWindowDeleted(BrowserWindow* window);

private Q_SLOTS:
    void load();

private:
    QString m_settingsPath;
    QHash<BrowserWindow*, DisableJS_Icon*> m_windows;
};

#endif // DISABLEJS_MANAGER_H
