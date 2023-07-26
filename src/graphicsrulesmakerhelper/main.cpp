/*
 * Graphics Rules Maker
 * Copyright (C) 2023 Wouter Haffmans <wouter@simply-life.net>
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
 *
 */

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QStandardPaths>

#include <iostream>
#include <memory>

#include "graphicsrulesmaker/videocarddatabase.h"
#include "graphicsrulesmaker/gamewriterfactory.h"
#include "graphicsrulesmaker/graphicsrulesmaker_config.h"
#include "graphicsrulesmaker/messagehandler.h"
#include "graphicsrulesmaker/graphicsruleswriter.h"

#include "helperclient.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("GraphicsRulesMaker");
    app.setApplicationVersion(GRAPHICSRULESMAKER_VERSION);
    app.setOrganizationName("SimsNetwork");
    app.setOrganizationDomain("simsnetwork.com");

    MessageHandler::setMessagePattern();
    QDir appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    appData.mkpath(".");
    MessageHandler logger(appData.filePath("log-helper.txt"));
    logger.install();

    qInfo() << qPrintable(app.applicationName()) << "(Helper)" << GRAPHICSRULESMAKER_VERSION << "starting";

    auto database = std::make_unique<VideoCardDatabase>();
    auto pluginFactory = std::make_unique<GameWriterFactory>();
    pluginFactory->loadPlugins();

    int result = app.exec();

    // TODO
    // - Connect to server
    // - Get plugin
    // - Get location + settings
    // - Get VC database
    // - Backup
    // - Write
    // - Close

    database.reset();
    pluginFactory.reset();

    qInfo() << "Exiting with code" << result;
    logger.uninstall();

    return result;
}
