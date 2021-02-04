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
#ifndef DISABLEJS_H
#define DISABLEJS_H

#include "plugininterface.h"

#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QListWidget>
#include <QPointer>

class BrowserWindow;
class DisableJS_Manager;
class DisableJS_Icon;

class DisableJS : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "Bhawk.Browser.plugin.DisableJS" FILE "disablejs.json")

public:
    explicit DisableJS();

    void init(InitState state, const QString &settingsPath) override;
    void unload() override;
    void showSettings(QWidget* parent = 0) override;
    bool testPlugin() override;

protected Q_SLOTS:
    bool r1();
    bool r2();
    bool removeFromList();
    bool onListItemClicked(QListWidgetItem* item);

private:
    QPointer<QDialog> m_settings;

    QString m_settingsPath;
    DisableJS_Manager* m_manager;
    QListWidgetItem* currentItem;

    bool saveSettings(const QString sType, const QString sValue);
};

#endif // DISABLEJS_H
