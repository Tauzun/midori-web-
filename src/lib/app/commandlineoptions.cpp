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
    QCommandLineOption authorsOption(QStringList({QStringLiteral("a"), QStringLiteral("authors")}));
    authorsOption.setDescription(QStringLiteral("Displays author information."));

    QCommandLineOption profileOption(QStringList({QStringLiteral("p"), QStringLiteral("profile")}));
    profileOption.setValueName(QStringLiteral("profileName"));
    profileOption.setDescription(QStringLiteral("Starts with the specified profile."));

    QCommandLineOption noExtensionsOption(QStringList({QStringLiteral("e"), QStringLiteral("no-plugs")}));
    noExtensionsOption.setDescription(QStringLiteral("Starts without loading plug-ins."));

    QCommandLineOption privateBrowsingOption(QStringList({QStringLiteral("i"), QStringLiteral("private")}));
    privateBrowsingOption.setDescription(QStringLiteral("Starts private browsing."));

    QCommandLineOption portableOption(QStringList({QStringLiteral("o"), QStringLiteral("portable")}));
    portableOption.setDescription(QStringLiteral("Starts in portable mode."));

    QCommandLineOption noRemoteOption(QStringList({QStringLiteral("r"), QStringLiteral("no-remote")}));
    noRemoteOption.setDescription(QStringLiteral("Starts a new browser instance."));

    QCommandLineOption newTabOption(QStringList({QStringLiteral("t"), QStringLiteral("new-tab")}));
    newTabOption.setDescription(QStringLiteral("Opens a new tab."));

    QCommandLineOption newWindowOption(QStringList({QStringLiteral("w"), QStringLiteral("new-win")}));
    newWindowOption.setDescription(QStringLiteral("Opens a new window."));

    QCommandLineOption downloadManagerOption(QStringList({QStringLiteral("d"), QStringLiteral("dl-man")}));
    downloadManagerOption.setDescription(QStringLiteral("Opens the download manager."));

    QCommandLineOption currentTabOption(QStringList({QStringLiteral("c"), QStringLiteral("cur-tab")}));
    currentTabOption.setValueName(QStringLiteral("URL"));
    currentTabOption.setDescription(QStringLiteral("Opens URL in the current tab."));

    QCommandLineOption openWindowOption(QStringList({QStringLiteral("u"), QStringLiteral("new-win")}));
    openWindowOption.setValueName(QStringLiteral("URL"));
    openWindowOption.setDescription(QStringLiteral("Opens URL in a new window."));

    QCommandLineOption fullscreenOption(QStringList({QStringLiteral("f"), QStringLiteral("fullscreen")}));
    fullscreenOption.setDescription(QStringLiteral("Toggles fullscreen."));

    QCommandLineOption wmclassOption(QStringList({QStringLiteral("wmclass")}));
    wmclassOption.setValueName(QStringLiteral("WM_CLASS"));
    wmclassOption.setDescription(QStringLiteral("Application class (X11 only)."));

    // Parser
    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("QtWebEngine based browser"));
    QCommandLineOption helpOption = parser.addHelpOption();
    QCommandLineOption versionOption = parser.addVersionOption();
    parser.addOption(authorsOption);
    parser.addOption(profileOption);
    parser.addOption(noExtensionsOption);
    parser.addOption(privateBrowsingOption);
    parser.addOption(portableOption);
    parser.addOption(noRemoteOption);
    parser.addOption(newTabOption);
    parser.addOption(newWindowOption);
    parser.addOption(downloadManagerOption);
    parser.addOption(currentTabOption);
    parser.addOption(openWindowOption);
    parser.addOption(fullscreenOption);
    parser.addOption(wmclassOption);
    parser.addPositionalArgument(QStringLiteral("URL"), QStringLiteral("URLs to open"), QStringLiteral("[URL...]"));

    // parse() and not process() so we can pass arbitrary options to Chromium
    parser.parse(QCoreApplication::arguments());

    if (parser.isSet(helpOption)) {
        parser.showHelp();
    }

    if (parser.isSet(versionOption)) {
        parser.showVersion();
    }

    if (parser.isSet(authorsOption)) {
        std::cout << "Astian <contact@astian.org>" << std::endl;

        ActionPair pair;
        pair.action = Qz::CL_ExitAction;
        m_actions.append(pair);
        return;
    }

    if (parser.isSet(profileOption)) {
        const QString profileName = parser.value(profileOption);
        std::cout << "midori: Starting with profile '" << profileName.toUtf8().data() << "'" << std::endl;

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

    if (parser.isSet(openWindowOption)) {
        ActionPair pair;
        pair.action = Qz::CL_OpenUrlInNewWindow;
        pair.text = parser.value(openWindowOption);
        m_actions.append(pair);
    }

    if (parser.isSet(fullscreenOption)) {
        ActionPair pair;
        pair.action = Qz::CL_ToggleFullScreen;
        m_actions.append(pair);
    }

    if (parser.isSet(wmclassOption)) {
        ActionPair pair;
        pair.action = Qz::CL_WMClass;
        pair.text = parser.value(wmclassOption);
        m_actions.append(pair);
    }

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
