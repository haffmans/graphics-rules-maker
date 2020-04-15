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

#ifndef SIMS2GAMEWRITER_H
#define SIMS2GAMEWRITER_H

#include <QtCore/QObject>
#include "graphicsrulesmaker/gamewriterinterface.h"

class Sims2Settings;
class DeviceModel;
class VideoCardDatabase;

class Sims2GameWriter : public QObject, public GameWriterInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.simsnetwork.GameWriterInterface")
    Q_INTERFACES(GameWriterInterface)

public:
    explicit Sims2GameWriter(QObject* parent = 0);

    virtual QString id() const {
        return "sims2";
    }

    virtual QString displayName() const {
        return tr("The Sims 2");
    }

    virtual QWidget* settingsWidget(DeviceModel *devices, VideoCardDatabase *database, QWidget* parent = 0);

    virtual QDir findGameDirectory() const;

    virtual QString gameExecutable() const { return "Sims2.exe"; }
    virtual QFileInfo gameExecutable(const QDir& gameDirectory) const;

    virtual QString rulesFileName() const { return "Graphics Rules.sgr"; }
    virtual QFileInfo rulesFileName(const QDir& gameDirectory) const;

    virtual QString databaseFileName() const { return "Video Cards.sgr"; }
    virtual QFileInfo databaseFileName(const QDir& gameDirectory) const;

    virtual void write(QWidget *settingsWidget, QIODevice* target);

    virtual ~Sims2GameWriter();

private:
    virtual QFileInfo findFile(QDir baseDir, QStringList options) const;
};

#endif // SIMS2GAMEWRITER_H
