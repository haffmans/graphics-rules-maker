/*
 * Graphics Rules Maker
 * Copyright (C) 2014-2020 Wouter Haffmans <wouter@simply-life.net>
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

#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include <QtCore/QDebug>
#include <QtCore/QStandardPaths>

#include <iostream>

#include "graphicsrulesmaker/devicemodel.h"
#include "graphicsrulesmaker/videocarddatabase.h"

#include "gamewriterfactory.h"
#include "mainwindow.h"
#include "messagehandler.h"
#include "graphicsrulesmaker/graphicsrulesmaker_config.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("GraphicsRulesMaker");
    app.setApplicationDisplayName("Graphics Rules Maker");
    app.setApplicationVersion(GRAPHICSRULESMAKER_VERSION);
    app.setOrganizationName("SimsNetwork");
    app.setOrganizationDomain("simsnetwork.com");

    MessageHandler::setMessagePattern();
    QDir appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    appData.mkpath(".");
    MessageHandler logger(appData.filePath("log.txt"));
    logger.install();

    qInfo() << qPrintable(app.applicationDisplayName()) << GRAPHICSRULESMAKER_VERSION << "starting";

    DeviceModel *model = new DeviceModel();
    VideoCardDatabase *database = new VideoCardDatabase();
    GameWriterFactory *pluginFactory = new GameWriterFactory();

    model->load();
    pluginFactory->loadPlugins();

    if (pluginFactory->plugins().size() == 0) {
        QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("No game plugins found. Please re-install the application."));
        return -1;
    }

    MainWindow window(model, database, pluginFactory);

    window.show();

    int result = app.exec();

    delete model;
    delete database;
    delete pluginFactory;

    qInfo() << "Exiting with code" << result;
    logger.uninstall();

    return result;
}
