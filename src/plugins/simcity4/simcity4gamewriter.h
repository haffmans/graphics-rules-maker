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

#ifndef SIMS2GAMEWRITER_H
#define SIMS2GAMEWRITER_H

#include <QtCore/QObject>
#include "graphicsrulesmaker/gamewriterinterface.h"

class SimCity4Settings;
class DeviceModel;
class VideoCardDatabase;

class SimCity4GameWriter : public QObject, public GameWriterInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.simsnetwork.GameWriterInterface")
    Q_INTERFACES(GameWriterInterface)

public:
    explicit SimCity4GameWriter(QObject* parent = 0);

    QString id() const override {
        return "simcity4";
    }

    QString displayName() const override {
        return tr("SimCity 4");
    }

    AbstractSettingsWidget* settingsWidget(DeviceModel *devices, VideoCardDatabase *database, QWidget* parent = 0) override;

    QDir findGameDirectory() const override;

    QString gameExecutable() const override { return "SimCity 4.exe"; }
    QFileInfo gameExecutable(const QDir& gameDirectory) const override;

    QString rulesFileName() const override { return "Graphics Rules.sgr"; }
    QFileInfo rulesFileName(const QDir& gameDirectory) const override;

    QString databaseFileName() const override { return "Video Cards.sgr"; }
    QFileInfo databaseFileName(const QDir& gameDirectory) const override;

    void write(const QVariantMap& settings, QIODevice* target) override;

    ~SimCity4GameWriter() override;

private:
    QFileInfo findFile(QDir baseDir, const QString& file) const;
};

#endif // SIMS2GAMEWRITER_H
