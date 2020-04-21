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

#ifndef SIMSPSGAMEWRITER_H
#define SIMSPSGAMEWRITER_H

#include <QtCore/QObject>
#include "graphicsrulesmaker/gamewriterinterface.h"

class SimsPSSettings;
class DeviceModel;
class VideoCardDatabase;

class SimsPSGameWriter : public QObject, public GameWriterInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.simsnetwork.GameWriterInterface")
    Q_INTERFACES(GameWriterInterface)

public:
    explicit SimsPSGameWriter(QObject* parent = 0);

    virtual QString id() const {
        return "sims-petstories";
    }

    virtual QString displayName() const {
        return tr("The Sims Pet Stories");
    }

    virtual QWidget* settingsWidget(DeviceModel *devices, VideoCardDatabase *database, QWidget* parent = 0);

    virtual QDir findGameDirectory() const;

    virtual QString gameExecutable() const { return "SimsPS.exe"; }
    virtual QFileInfo gameExecutable(const QDir& gameDirectory) const;

    virtual QString rulesFileName() const { return "Graphics Rules.sgr"; }
    virtual QFileInfo rulesFileName(const QDir& gameDirectory) const;

    virtual QString databaseFileName() const { return "Video Cards.sgr"; }
    virtual QFileInfo databaseFileName(const QDir& gameDirectory) const;

    virtual void write(QWidget *settingsWidget, QIODevice* target);

    virtual ~SimsPSGameWriter();

private:
    virtual QFileInfo findFile(QDir baseDir, QStringList options) const;
};

#endif // SIMSPSGAMEWRITER_H
