/*
 * Graphics Rules Maker
 * Copyright (C) 2014 Wouter Haffmans <wouter@simply-life.net>
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
 */

#include "gamewriterfactory.h"

#include <QtCore/QMap>
#include <QtCore/QLibrary>
#include <QtCore/QPluginLoader>
#include <QtCore/QtDebug>

#include "graphicsrulesmaker/graphicsrulesmaker_config.h"
#include "graphicsrulesmaker/gamewriterinterface.h"


GameWriterFactory::GameWriterFactory(QObject* parent)
    : QObject(parent)
{
#ifdef Q_OS_WIN32
    // Scan in directory of self application
    m_searchDirectories.append(QDir::current().absoluteFilePath("gamewriters"));
    m_searchDirectories.append(QDir::current().absoluteFilePath("bin/gamewriters"));
#endif

#if !defined(Q_OS_WIN32) || !defined(NDEBUG)
    // Useful on Win32 while debugging, but not suitable for release builds on Win32.
    // This'll be the installation path on Linux (etc) platforms, so it's alright there.
    m_searchDirectories.append(QDir(GRAPHICSRULESMAKER_PLUGIN_PATH));
#endif
}

void GameWriterFactory::loadPlugins()
{
    QMap<QString, QDir> libraries; // filename -> searchdir
    foreach(const QDir &searchDir, m_searchDirectories) {
        qDebug() << "Scanning directory " << searchDir.absolutePath() << " for plugins...";
        QStringList files = searchDir.entryList(QDir::Files);
        foreach(const QString &pluginFile, files) {
            if (!libraries.contains(pluginFile)) {
                libraries.insert(pluginFile, searchDir);
            }
        }
    }

    QMap<QString, QDir>::const_iterator i;
    for (i = libraries.constBegin(); i != libraries.constEnd(); ++i) {
        QString fullPath = i.value().absoluteFilePath(i.key());
        QPluginLoader loader(fullPath, this);
        GameWriterInterface *instance = dynamic_cast<GameWriterInterface*>(loader.instance());
        if (instance) {
            qDebug() << "Found plugin for game " << instance->displayName() << " [" << instance->id() << "] in " << fullPath;
            m_plugins.append(instance);
            m_pluginPaths.insert(instance->id(), fullPath);
        }
    }
}

QList<GameWriterInterface*> GameWriterFactory::plugins() const
{
    return m_plugins;
}

GameWriterInterface* GameWriterFactory::plugin(const QString& id) const
{
    auto writerIter = std::find_if<QList<GameWriterInterface*>::const_iterator>(m_plugins.constBegin(), m_plugins.constEnd(),
        [id] (GameWriterInterface* writer) { return writer->id() == id; }
    );

    if (writerIter == m_plugins.constEnd()) {
        return nullptr;
    }
    return *writerIter;
}

QStringList GameWriterFactory::pluginNames() const
{
    QStringList result;

    foreach(GameWriterInterface* plugin, m_plugins) {
        result.append(plugin->displayName());
    }

    return result;
}

QString GameWriterFactory::translationDirectory(const QString & id) const
{
    if (!m_pluginPaths.contains(id)) {
        return QString();
    }
    QFileInfo info(m_pluginPaths.value(id));
    return info.absolutePath();
}

QString GameWriterFactory::translationFilename(const QString& id) const
{
    if (!m_pluginPaths.contains(id)) {
        return QString();
    }
    QFileInfo info(m_pluginPaths.value(id));
    return info.baseName();
}

GameWriterFactory::~GameWriterFactory()
{
}

#include "gamewriterfactory.moc"
