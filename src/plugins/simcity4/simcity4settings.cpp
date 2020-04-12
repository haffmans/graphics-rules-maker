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

#include "simcity4settings.h"
#include "ui_simcity4settings.h"

#include <QtCore/QSettings>

#include "graphicsrulesmaker/devicemodel.h"
#include "graphicsrulesmaker/videocarddatabase.h"

SimCity4Settings::SimCity4Settings(DeviceModel *devices, VideoCardDatabase *database, QWidget* parent)
    : QWidget(parent)
{
    ui = new Ui::SimCity4Settings;
    ui->setupUi(this);

    connect(ui->resetDefaults, &QPushButton::clicked, this, &SimCity4Settings::reset);

    // Load available resolutions
    QStringList resolutions;
    int previousWidth = -1;
    int previousHeight = -1;
    foreach(const GraphicsMode &mode, devices->allModes()) {
        // We don't care about refresh rates - i.e. avoid duplicates from the list
        if (mode.width != previousWidth || mode.height != previousHeight) {
            resolutions << QString::number(mode.width) + "x" + QString::number(mode.height);
            previousWidth = mode.width;
            previousHeight = mode.height;
        }
    }

    // Load Settings
    QSettings s;
    s.beginGroup("simcity4");

    ui->radeonHd7000Fix->setChecked(s.value("radeonHd7000Fix", false).toBool());
    ui->fastCard->setChecked(s.value("fastCard", false).toBool());
    ui->allHighSettings->setChecked(s.value("allHighSettings", false).toBool());

    s.endGroup();
}

SimCity4Variables SimCity4Settings::current() const
{
    SimCity4Variables result;
    result.radeonHd7000Fix = ui->radeonHd7000Fix->isChecked();
    result.fastCard = ui->fastCard->isChecked();
    result.allHighSettings = ui->allHighSettings->isChecked();
    return result;
}

void SimCity4Settings::reset()
{
    // Restore all defaults
    ui->radeonHd7000Fix->setChecked(false);
    ui->fastCard->setChecked(false);
    ui->allHighSettings->setChecked(false);
}

SimCity4Settings::~SimCity4Settings()
{
    // Write settings
    QSettings s;
    SimCity4Variables vars = current();

    s.beginGroup("simcity4");
    s.setValue("radeonHd7000Fix", ui->radeonHd7000Fix->isChecked());
    s.setValue("fastCard", ui->fastCard->isChecked());
    s.setValue("allHighSettings", ui->allHighSettings->isChecked());
    s.endGroup();

    delete ui;
}

#include "moc_simcity4settings.cpp"
