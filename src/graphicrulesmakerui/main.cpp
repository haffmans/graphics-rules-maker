/*
 * Graphic Rules Maker
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

#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>

#include "graphicrulesmaker/devicemodel.h"
#include "graphicrulesmaker/videocarddatabase.h"

#include "gamewriterfactory.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("GraphicRulesMaker");
    app.setApplicationDisplayName("Graphics Rules Maker");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("SimsNetwork");
    app.setOrganizationDomain("simsnetwork.com");

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

    return result;
}
