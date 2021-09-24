/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2018 David Rosca <nowrep@gmail.com>
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
#include "pluginproxy.h"
#include "plugininterface.h"
#include "mainapplication.h"
#include "speeddial.h"
#include "settings.h"
#include "datapaths.h"
#include "adblock/adblockplugin.h"
#include "../config.h"
#include "desktopfile.h"
#include "qml/qmlplugins.h"
#include "qml/qmlplugin.h"

#include <iostream>

#include <QPluginLoader>
#include <QDir>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QFileInfo>
#include <QSettings>

bool Plugins::Plugin::isLoaded() const
{
    return instance;
}

bool Plugins::Plugin::isRemovable() const
{
    return !pluginPath.isEmpty() && QFileInfo(pluginPath).isWritable();
}

bool Plugins::Plugin::operator==(const Plugin &other) const
{
    return type == other.type && pluginId == other.pluginId;
}

Plugins::Plugins(QObject* parent)
    : QObject(parent)
    , m_pluginsLoaded(false)
    , m_speedDial(new SpeedDial(this))
{
    loadSettings();

    if (!MainApplication::isTestModeEnabled()) {
        loadPythonSupport();
    }
}

QList<Plugins::Plugin> Plugins::availablePlugins()
{
    loadAvailablePlugins();
    return m_availablePlugins;
}

bool Plugins::loadPlugin(Plugins::Plugin* plugin)
{
    if (plugin->isLoaded()) {
        return true;
    }

    if (!initPlugin(PluginInterface::LateInitState, plugin)) {
        return false;
    }

    m_availablePlugins.removeOne(*plugin);
    m_availablePlugins.prepend(*plugin);

    refreshLoadedPlugins();

    return plugin->isLoaded();
}

void Plugins::unloadPlugin(Plugins::Plugin* plugin)
{
    if (!plugin->isLoaded()) {
        return;
    }

    plugin->instance->unload();
    emit pluginUnloaded(plugin->instance);
    plugin->instance = nullptr;

    m_availablePlugins.removeOne(*plugin);
    m_availablePlugins.append(*plugin);

    refreshLoadedPlugins();
}

void Plugins::removePlugin(Plugins::Plugin *plugin)
{
    if (!plugin->isRemovable()) {
        return;
    }

    if (plugin->isLoaded()) {
        unloadPlugin(plugin);
    }

    bool result = false;

    QFileInfo info(plugin->pluginPath);
    if (info.isDir()) {
        result = QDir(plugin->pluginPath).removeRecursively();
    } else if (info.isFile()) {
        result = QFile::remove(plugin->pluginPath);
    }

    if (!result) {
        qWarning() << "Failed to remove" << plugin->pluginSpec.name;
        return;
    }

    m_availablePlugins.removeOne(*plugin);
    emit availablePluginsChanged();
}

bool Plugins::addPlugin(const QString &id)
{
    Plugin plugin = loadPlugin(id);
    if (plugin.type == Plugin::Invalid) {
        return false;
    }
    if (plugin.pluginSpec.name.isEmpty()) {
        qWarning() << "Invalid plugin spec of" << id << "plugin";
        return false;
    }
    registerAvailablePlugin(plugin);
    emit availablePluginsChanged();
    return true;
}

void Plugins::loadSettings()
{
    QStringList defaultAllowedPlugins = {
        QStringLiteral("internal:adblock")
    };

    #ifdef Q_OS_UNIX
        // Enable DisableJS by default
        defaultAllowedPlugins.append(QStringLiteral("lib:DisableJS.so"));
        // Enable KDE Frameworks Integration when running inside KDE session
        if (qgetenv("KDE_FULL_SESSION") == QByteArray("true")) {
            defaultAllowedPlugins.append(QStringLiteral("lib:KDEFrameworksIntegration.so"));
        }
    #endif

    #ifdef Q_OS_WIN
        // Enable DisableJS by default
        defaultAllowedPlugins.append(QStringLiteral("lib:DisableJS.dll"));
    #endif

    Settings settings;
    m_allowedPlugins = settings.value("Plugin-Settings/AllowedPlugins", defaultAllowedPlugins).toStringList();
}

void Plugins::shutdown()
{
    for (PluginInterface* iPlugin : qAsConst(m_loadedPlugins)) {
        iPlugin->unload();
    }
}

PluginSpec Plugins::createSpec(const QJsonObject &metaData)
{
    const QString tempIcon = DataPaths::path(DataPaths::Temp) + QLatin1String("/icon");
    const QString tempMetadata = DataPaths::path(DataPaths::Temp) + QLatin1String("/metadata.desktop");
    QFile::remove(tempIcon);
    QFile::remove(tempMetadata);
    QSettings settings(tempMetadata, QSettings::IniFormat);
    settings.beginGroup(QStringLiteral("Desktop Entry"));
    for (QJsonObject::const_iterator it = metaData.begin(); it != metaData.end(); ++it) {
        const QString value = it.value().toString();
        if (it.key() == QLatin1String("Icon") && value.startsWith(QLatin1String("base64:"))) {
            QFile file(tempIcon);
            if (file.open(QFile::WriteOnly)) {
                file.write(QByteArray::fromBase64(value.mid(7).toUtf8()));
                settings.setValue(it.key(), tempIcon);
            }
        } else {
            settings.setValue(it.key(), it.value().toString());
        }
    }
    settings.sync();
    return createSpec(DesktopFile(tempMetadata));
}

PluginSpec Plugins::createSpec(const DesktopFile &metaData)
{
    PluginSpec spec;
    spec.name = metaData.name();
    spec.description = metaData.comment();
    spec.version = metaData.value(QStringLiteral("X-Bhawk-Version")).toString();
    spec.author = QStringLiteral("%1 <%2>").arg(metaData.value(QStringLiteral("X-Bhawk-Author")).toString(), metaData.value(QStringLiteral("X-Bhawk-Email")).toString());
    spec.hasSettings = metaData.value(QStringLiteral("X-Bhawk-Settings")).toBool();

    const QString iconName = metaData.icon();
    if (!iconName.isEmpty()) {
        if (QFileInfo::exists(iconName)) {
            spec.icon = QIcon(iconName).pixmap(32);
        } else {
            const QString relativeFile = QFileInfo(metaData.fileName()).dir().absoluteFilePath(iconName);
            if (QFileInfo::exists(relativeFile)) {
                spec.icon = QIcon(relativeFile).pixmap(32);
            } else {
                spec.icon = QIcon::fromTheme(iconName).pixmap(32);
            }
        }
    }

    return spec;
}

void Plugins::loadPlugins()
{
    QDir settingsDir(DataPaths::currentProfilePath() + "/plug-ins/");
    if (!settingsDir.exists()) {
        settingsDir.mkdir(settingsDir.absolutePath());
    }

    for (const QString &pluginId : qAsConst(m_allowedPlugins)) {
        Plugin plugin = loadPlugin(pluginId);
        if (plugin.type == Plugin::Invalid) {
            continue;
        }
        if (plugin.pluginSpec.name.isEmpty()) {
            qWarning() << "Invalid plugin spec of" << pluginId << "plugin";
            continue;
        }
        if (!initPlugin(PluginInterface::StartupInitState, &plugin)) {
            qWarning() << "Failed to init" << pluginId << "plugin";
            continue;
        }
        registerAvailablePlugin(plugin);
    }

    refreshLoadedPlugins();

    std::cout << "Midori Browser: " << m_loadedPlugins.count() << " plug-ins loaded"  << std::endl;
}

void Plugins::loadAvailablePlugins()
{
    if (m_pluginsLoaded) {
        return;
    }

    m_pluginsLoaded = true;

    const QStringList dirs = DataPaths::allPaths(DataPaths::Plugins);

    // InternalPlugin
    registerAvailablePlugin(loadInternalPlugin(QStringLiteral("adblock")));

    for (const QString &dir : dirs) {
        const QList<QFileInfo> files = QDir(dir).entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QFileInfo &info : files) {
            Plugin plugin;
            const QString pluginPath = info.absoluteFilePath();
            if (info.isFile() && QLibrary::isLibrary(pluginPath)) {
                // SharedLibraryPlugin
                if (info.baseName() != QLatin1String("PyBhawk")) {
                    plugin = loadSharedLibraryPlugin(pluginPath);
                }
            } else if (info.isDir()) {
                const DesktopFile metaData(QDir(pluginPath).filePath(QStringLiteral("metadata.desktop")));
                const QString type = metaData.value(QStringLiteral("X-Bhawk-Type")).toString();
                if (type == QLatin1String("Plug-in/Python")) {
                    // PythonPlugin
                    plugin = loadPythonPlugin(pluginPath);
                } else if (type == QLatin1String("Plug-in/Qml")) {
                    // QmlPlugin
                    plugin = QmlPlugin::loadPlugin(pluginPath);
                } else {
                    qWarning() << "Invalid type" << type << "of" << pluginPath << "plugin";
                }
            }
            if (plugin.type == Plugin::Invalid) {
                continue;
            }
            if (plugin.pluginSpec.name.isEmpty()) {
                qWarning() << "Invalid plugin spec of" << pluginPath << "plugin";
                continue;
            }
            registerAvailablePlugin(plugin);
        }
    }
}

void Plugins::registerAvailablePlugin(const Plugin &plugin)
{
    if (!m_availablePlugins.contains(plugin)) {
        m_availablePlugins.append(plugin);
    }
}

void Plugins::refreshLoadedPlugins()
{
    m_loadedPlugins.clear();

    for (const Plugin &plugin : qAsConst(m_availablePlugins)) {
        if (plugin.isLoaded()) {
            m_loadedPlugins.append(plugin.instance);
        }
    }

    emit availablePluginsChanged();
}

void Plugins::loadPythonSupport()
{
    const QStringList dirs = DataPaths::allPaths(DataPaths::Plugins);
    for (const QString &dir : dirs) {
        const QList<QFileInfo> files = QDir(dir).entryInfoList({QSL("PyBhawk*")}, QDir::Files);
        for (const QFileInfo &info : files) {
            m_pythonPlugin = new QLibrary(info.absoluteFilePath(), this);
            m_pythonPlugin->setLoadHints(QLibrary::ExportExternalSymbolsHint);
            if (!m_pythonPlugin->load()) {
                qWarning() << "Failed to load python support plugin" << m_pythonPlugin->errorString();
                delete m_pythonPlugin;
                m_pythonPlugin = nullptr;
            } else {
                std::cout << "Midori Browser: Python plugin support initialized" << std::endl;
                return;
            }
        }
    }
}

Plugins::Plugin Plugins::loadPlugin(const QString &id)
{
    QString name;
    Plugin::Type type = Plugin::Invalid;

    const int colon = id.indexOf(QLatin1Char(':'));
    if (colon > -1) {
        const QStringRef t = id.leftRef(colon);
        if (t == QLatin1String("internal")) {
            type = Plugin::InternalPlugin;
        } else if (t == QLatin1String("lib")) {
            type = Plugin::SharedLibraryPlugin;
        } else if (t == QLatin1String("python")) {
            type = Plugin::PythonPlugin;
        } else if (t == QLatin1String("qml")) {
            type = Plugin::QmlPlugin;
        }
        name = id.mid(colon + 1);
    } else {
        name = id;
        type = Plugin::SharedLibraryPlugin;
    }

    switch (type) {
    case Plugin::InternalPlugin:
        return loadInternalPlugin(name);

    case Plugin::SharedLibraryPlugin:
        return loadSharedLibraryPlugin(name);

    case Plugin::PythonPlugin:
        return loadPythonPlugin(name);

    case Plugin::QmlPlugin:
        return QmlPlugin::loadPlugin(name);

    default:
        return Plugin();
    }
}

Plugins::Plugin Plugins::loadInternalPlugin(const QString &name)
{
    if (name == QLatin1String("adblock")) {
        Plugin plugin;
        plugin.type = Plugin::InternalPlugin;
        plugin.pluginId = QStringLiteral("internal:adblock");
        plugin.internalInstance = new AdBlockPlugin();
        plugin.pluginSpec = createSpec(DesktopFile(QStringLiteral(":adblock/metadata.desktop")));
        return plugin;
    } else {
        return Plugin();
    }
}

Plugins::Plugin Plugins::loadSharedLibraryPlugin(const QString &name)
{
    QString fullPath;
    if (QFileInfo(name).isAbsolute()) {
        fullPath = name;
    } else {
        fullPath = DataPaths::locate(DataPaths::Plugins, name);
        if (fullPath.isEmpty()) {
            qWarning() << "Library plugin" << name << "not found";
            return Plugin();
        }
    }

    Plugin plugin;
    plugin.type = Plugin::SharedLibraryPlugin;
    plugin.pluginId = QStringLiteral("lib:%1").arg(QFileInfo(fullPath).fileName());
    plugin.pluginPath = fullPath;
    plugin.pluginLoader = new QPluginLoader(fullPath);
    plugin.pluginSpec = createSpec(plugin.pluginLoader->metaData().value(QStringLiteral("MetaData")).toObject());
    return plugin;
}

Plugins::Plugin Plugins::loadPythonPlugin(const QString &name)
{
    Plugin out;

    if (!m_pythonPlugin) {
        qWarning() << "Python support plugin is not loaded";
        return out;
    }

    std::function<Plugins::Plugin *(const QString &)> f = (Plugin*(*)(const QString &)) m_pythonPlugin->resolve("pybhawk_load_plugin");

    if (!f) {
        qWarning() << "Failed to resolve" << "pybhawk_load_plugin";
        return out;
    }

    Plugin *p = f(name);
    if (p) {
        out = *p;
        delete p;
    }

    return out;
}

bool Plugins::initPlugin(PluginInterface::InitState state, Plugin *plugin)
{
    if (!plugin) {
        return false;
    }

    switch (plugin->type) {
    case Plugin::InternalPlugin:
        initInternalPlugin(plugin);
        break;

    case Plugin::SharedLibraryPlugin:
        initSharedLibraryPlugin(plugin);
        break;

    case Plugin::PythonPlugin:
        initPythonPlugin(plugin);
        break;

    case Plugin::QmlPlugin:
        QmlPlugin::initPlugin(plugin);
        break;

    default:
        return false;
    }

    if (!plugin->instance) {
        return false;
    }

    // DataPaths::currentProfilePath() + QLatin1String("/plug-ins") is duplicated in qmlsettings.cpp
    // If you change this, please change it there too.
    plugin->instance->init(state, DataPaths::currentProfilePath() + QLatin1String("/plug-ins"));

    if (!plugin->instance->testPlugin()) {
        emit pluginUnloaded(plugin->instance);
        plugin->instance = nullptr;
        return false;
    }

    return true;
}

void Plugins::initInternalPlugin(Plugin *plugin)
{
    Q_ASSERT(plugin->type == Plugin::InternalPlugin);

    plugin->instance = plugin->internalInstance;
}

void Plugins::initSharedLibraryPlugin(Plugin *plugin)
{
    Q_ASSERT(plugin->type == Plugin::SharedLibraryPlugin);

    plugin->instance = qobject_cast<PluginInterface*>(plugin->pluginLoader->instance());

    if (!plugin->instance) {
        qWarning() << "Loading" << plugin->pluginPath << "failed:" << plugin->pluginLoader->errorString();
    }
}

void Plugins::initPythonPlugin(Plugin *plugin)
{
    Q_ASSERT(plugin->type == Plugin::PythonPlugin);

    if (!m_pythonPlugin) {
        qWarning() << "Python support plugin is not loaded";
        return;
    }

    std::function<void (Plugins::Plugin *)> f = (void(*)(Plugin *)) m_pythonPlugin->resolve("pybhawk_init_plugin");
    if (!f) {
        qWarning() << "Failed to resolve" << "pybhawk_init_plugin";
        return;
    }

    f(plugin);
}
