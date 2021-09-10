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
#ifndef DISABLEJS_ICON_H
#define DISABLEJS_ICON_H

#include "plugininterface.h"
#include "abstractbuttoninterface.h"
#include "datapaths.h"

#include <QPointer>

class DisableJS_Manager;
class BrowserWindow;

class WebView;

class DisableJS_Icon : public AbstractButtonInterface
{
    Q_OBJECT

public:
    explicit DisableJS_Icon(DisableJS_Manager *manager);

private Q_SLOTS:
    void toggleJs();

private:
    DisableJS_Manager *m_manager;
    QString id() const override;
    QString name() const override;
    QString modeCheck(QString sP);
    QString m_settingsPath = DataPaths::currentProfilePath() + QLatin1String("/plug-ins");
    QPointer<WebView> m_view;
    void updateState();
    void toggleAddRemove();
    void webViewChanged(WebView *view);
};

#endif // DISABLEJS_ICON_H
