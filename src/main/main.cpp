/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2017 David Rosca <nowrep@gmail.com>
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
#include "mainapplication.h"
#include "proxystyle.h"

#include <iostream>

#ifndef Q_OS_WIN
void msgHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if (msg.startsWith(QLatin1String("QSslSocket: cannot resolve SSL")))
        return;
    if (msg.startsWith(QLatin1String("Remote debugging server started successfully.")))
        return;

    switch (type) {
    case QtDebugMsg:
    case QtInfoMsg:
    case QtWarningMsg:
    case QtCriticalMsg:
        std::cerr << qPrintable(qFormatLogMessage(type, context, msg)) << std::endl;
        break;

    case QtFatalMsg:
        std::cerr << "Fatal: " << qPrintable(qFormatLogMessage(type, context, msg)) << std::endl;
        abort();

    default:
        break;
    }
}
#endif

int main(int argc, char* argv[])
{

#ifdef Q_OS_LINUX
    const QByteArray cDesk = qgetenv("XDG_CURRENT_DESKTOP").toLower();
    const QByteArray sessDesk = qgetenv("XDG_SESSION_DESKTOP").toLower();
    const QByteArray sessType = qgetenv("XDG_SESSION_TYPE").toLower();
    if (sessType.contains("wayland") && (cDesk.contains("gnome") || sessDesk.contains("gnome"))) {
        const QByteArray platBa = QString("xcb").toLatin1();
        qputenv("QT_QPA_PLATFORM", platBa); // No Wayland support yet. Sorry
    }
#endif

#ifndef Q_OS_WIN
    qInstallMessageHandler(&msgHandler);
#endif
    // Hack to fix QT_STYLE_OVERRIDE with QProxyStyle
    const QByteArray style = qgetenv("QT_STYLE_OVERRIDE");
    if (!style.isEmpty()) {
        char** args = (char**) malloc(sizeof(char*) * (argc + 1));
        for (int i = 0; i < argc; ++i)
            args[i] = argv[i];

        QString stylecmd = QLatin1String("-style=") + style;
        args[argc++] = qstrdup(stylecmd.toUtf8().constData());
        argv = args;
    }

    MainApplication::setOrganizationName("org");
    MainApplication::setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents);
    MainApplication::setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents);
    MainApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    MainApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    MainApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    MainApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);

    MainApplication app(argc, argv);

    if (app.isClosing())
        return 0;

#ifndef Q_OS_WIN
    app.setStyle("windowsvista"); // Attempt to fix ugly native theme styles
#endif
    app.setProxyStyle(new ProxyStyle);

    return app.exec();
}
