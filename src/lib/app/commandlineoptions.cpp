/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2016 David Rosca <nowrep@gmail.com>
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
#include "commandlineoptions.h"

#include <QFileInfo>
#include <QCoreApplication>
#include <QCommandLineParser>

#include <iostream>

CommandLineOptions::CommandLineOptions()
{
    parseActions();
}

CommandLineOptions::ActionPairList CommandLineOptions::getActions()
{
    return m_actions;
}

void CommandLineOptions::parseActions()
{
    // Options
    QCommandLineOption profileOption(QStringList({QStringLiteral("a"), QStringLiteral("opt-a")}));
    profileOption.setValueName(QStringLiteral("profileName"));
    profileOption.setDescription(QStringLiteral("Start by loading the specified profile."));

    QCommandLineOption noExtensionsOption(QStringList({QStringLiteral("b"), QStringLiteral("opt-b")}));
    noExtensionsOption.setDescription(QStringLiteral("Start without loading any plug-ins."));

    QCommandLineOption privateBrowsingOption(QStringList({QStringLiteral("c"), QStringLiteral("opt-c")}));
    privateBrowsingOption.setDescription(QStringLiteral("Start private browsing."));

    QCommandLineOption portableOption(QStringList({QStringLiteral("e"), QStringLiteral("opt-e")}));
    portableOption.setDescription(QStringLiteral("Start in portable mode."));

    QCommandLineOption noRemoteOption(QStringList({QStringLiteral("f"), QStringLiteral("opt-f")}));
    noRemoteOption.setDescription(QStringLiteral("Start a new browser instance."));

    QCommandLineOption newWindowOption(QStringList({QStringLiteral("i"), QStringLiteral("opt-i")}));
    newWindowOption.setDescription(QStringLiteral("Open a new window."));

    QCommandLineOption downloadManagerOption(QStringList({QStringLiteral("j"), QStringLiteral("opt-j")}));
    downloadManagerOption.setDescription(QStringLiteral("Open the download manager."));

    QCommandLineOption currentTabOption(QStringList({QStringLiteral("k"), QStringLiteral("opt-k")}));
    currentTabOption.setValueName(QStringLiteral("URL"));
    currentTabOption.setDescription(QStringLiteral("Open URL in the current tab."));

    QCommandLineOption fullscreenOption(QStringList({QStringLiteral("m"), QStringLiteral("opt-m")}));
    fullscreenOption.setDescription(QStringLiteral("Start in fullscreen."));

    QCommandLineOption newTabOption(QStringList({QStringLiteral("u"), QStringLiteral("opt-u")}));
    newTabOption.setDescription(QStringLiteral("Open a new tab."));
#ifdef Q_OS_LINUX
    QCommandLineOption wmclassOption(QStringList({QStringLiteral("wmclass")}));
    wmclassOption.setValueName(QStringLiteral("WM_CLASS"));
    wmclassOption.setDescription(QStringLiteral("Application class (X11 only)."));
#endif
    // Parser
    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Midori Web Browser"));
    parser.addOption(profileOption);
    parser.addOption(noExtensionsOption);
    parser.addOption(privateBrowsingOption);
    parser.addOption(portableOption);
    parser.addOption(noRemoteOption);
    QCommandLineOption helpOption = parser.addHelpOption();
    parser.addOption(newWindowOption);
    parser.addOption(downloadManagerOption);
    parser.addOption(currentTabOption);
    parser.addOption(fullscreenOption);
    parser.addOption(newTabOption);
    QCommandLineOption versionOption = parser.addVersionOption();
#ifdef Q_OS_LINUX
    parser.addOption(wmclassOption);
#endif
    parser.addPositionalArgument(QStringLiteral("URL"), QStringLiteral("URLs to open"), QStringLiteral("[URL...]"));
    // parse() and not process() so we can pass arbitrary options to Chromium
    parser.parse(QCoreApplication::arguments());

    if (parser.isSet(helpOption)) {
        parser.showHelp();
    }

    if (parser.isSet(versionOption)) {
        parser.showVersion();
    }

    if (parser.isSet(profileOption)) {
        const QString profileName = parser.value(profileOption);
        std::cout << "browser: Starting with profile '" << profileName.toUtf8().data() << "'" << std::endl;

        ActionPair pair;
        pair.action = Qz::CL_StartWithProfile;
        pair.text = profileName;
        m_actions.append(pair);
    }

    if (parser.isSet(noExtensionsOption)) {
        ActionPair pair;
        pair.action = Qz::CL_StartWithoutAddons;
        m_actions.append(pair);
    }

    if (parser.isSet(privateBrowsingOption)) {
        ActionPair pair;
        pair.action = Qz::CL_StartPrivateBrowsing;
        m_actions.append(pair);
    }

    if (parser.isSet(portableOption)) {
        ActionPair pair;
        pair.action = Qz::CL_StartPortable;
        m_actions.append(pair);
    }

    if (parser.isSet(noRemoteOption)) {
        ActionPair pair;
        pair.action = Qz::CL_StartNewInstance;
        m_actions.append(pair);
    }

    if (parser.isSet(newTabOption)) {
        ActionPair pair;
        pair.action = Qz::CL_NewTab;
        m_actions.append(pair);
    }

    if (parser.isSet(newWindowOption)) {
        ActionPair pair;
        pair.action = Qz::CL_NewWindow;
        m_actions.append(pair);
    }

    if (parser.isSet(downloadManagerOption)) {
        ActionPair pair;
        pair.action = Qz::CL_ShowDownloadManager;
        m_actions.append(pair);
    }

    if (parser.isSet(currentTabOption)) {
        ActionPair pair;
        pair.action = Qz::CL_OpenUrlInCurrentTab;
        pair.text = parser.value(currentTabOption);
        m_actions.append(pair);
    }

    if (parser.isSet(fullscreenOption)) {
        ActionPair pair;
        pair.action = Qz::CL_ToggleFullScreen;
        m_actions.append(pair);
    }
#ifdef Q_OS_LINUX
    if (parser.isSet(wmclassOption)) {

        const QByteArray sessType = qgetenv("XDG_SESSION_TYPE").toLower();

        if (!sessType.contains("wayland")) {
            ActionPair pair;
            pair.action = Qz::CL_WMClass;
            pair.text = parser.value(wmclassOption);
            m_actions.append(pair);
        }

    }
#endif
    if (parser.positionalArguments().isEmpty())
        return;

    QString url = parser.positionalArguments().constLast();
    QFileInfo fileInfo(url);

    if (fileInfo.exists()) {
        url = fileInfo.absoluteFilePath();
    }

    if (!url.isEmpty() && !url.startsWith(QLatin1Char('-'))) {
        ActionPair pair;
        pair.action = Qz::CL_OpenUrl;
        pair.text = url;
        m_actions.append(pair);
    }
}
