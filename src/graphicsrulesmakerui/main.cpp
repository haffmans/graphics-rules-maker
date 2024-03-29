/*
 * Graphics Rules Maker
 * Copyright (C) 2014-2021 Wouter Haffmans <wouter@simply-life.net>
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
#include <QtGui/QPalette>
#include <QtGui/QStyleHints>
#include <QtCore/QDebug>
#include <QtCore/QStandardPaths>

#include <iostream>
#include <memory>

#include "graphicsrulesmaker/devicemodel.h"
#include "graphicsrulesmaker/videocarddatabase.h"
#include "graphicsrulesmaker/gamewriterfactory.h"
#include "graphicsrulesmaker/graphicsruleswriter.h"
#include "graphicsrulesmaker/messagehandler.h"

#include "mainwindow.h"
#include "graphicsrulesmaker/graphicsrulesmaker_config.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("GraphicsRulesMaker");
    app.setApplicationDisplayName("Graphics Rules Maker");
    app.setApplicationVersion(GRAPHICSRULESMAKER_VERSION);
    app.setOrganizationName("SimsNetwork");
    app.setOrganizationDomain("simsnetwork.com");

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    // Link color is ugly by default in dark mode -> use bright text color instead
    if (app.styleHints()->colorScheme() == Qt::ColorScheme::Dark) {
        auto palette = app.palette();
        palette.setColor(QPalette::Link, palette.brightText().color());
        app.setPalette(palette);
    }
#endif

    MessageHandler::setMessagePattern();
    QDir appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    appData.mkpath(".");
    MessageHandler logger(appData.filePath("log.txt"));
    logger.install();

    qInfo() << qPrintable(app.applicationDisplayName()) << GRAPHICSRULESMAKER_VERSION << "starting";

    auto model = std::make_unique<DeviceModel>();
    auto database = std::make_unique<VideoCardDatabase>();
    auto pluginFactory = std::make_unique<GameWriterFactory>();
    auto writer = std::make_unique<GraphicsRulesWriter>(pluginFactory.get(), database.get());

    model->load();
    pluginFactory->loadPlugins();

    int result = 0;
    {
        // MainWindow must be created prior to showing any messages, otherwise they won't be translated
        MainWindow window(model.get(), pluginFactory.get(), writer.get());

        if (pluginFactory->plugins().size() == 0) {
            QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("No game plugins found. Please re-install the application."));
            return 1;
        }
        if (model->rowCount() == 0) {
            QMessageBox::warning(0, QObject::tr("No Graphics Card"),
                QObject::tr("No DirectX supported graphics card detected. Please install the latest drivers for your graphics card. Graphics Rules Maker may not function correctly."));
        }

        window.show();

        result = app.exec();
    }

    writer.reset();
    model.reset();
    database.reset();
    pluginFactory.reset();

    qInfo() << "Exiting with code" << result;
    logger.uninstall();

    return result;
}
