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
#include "simcity4variables.h"

SimCity4Settings::SimCity4Settings(DeviceModel *devices, VideoCardDatabase *database, QWidget* parent)
    : AbstractSettingsWidget(parent)
    , ui(new Ui::SimCity4Settings)
{
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
}

QVariantMap SimCity4Settings::settings() const
{
    SimCity4Variables options;
    options.radeonHd7000Fix = ui->radeonHd7000Fix->isChecked();
    options.fastCard = ui->fastCard->isChecked();
    options.allHighSettings = ui->allHighSettings->isChecked();
    return options;
}

void SimCity4Settings::setSettings(const QVariantMap& settings)
{
    SimCity4Variables options(settings);

    ui->radeonHd7000Fix->setChecked(options.radeonHd7000Fix);
    ui->fastCard->setChecked(options.fastCard);
    ui->allHighSettings->setChecked(options.allHighSettings);
}

void SimCity4Settings::reset()
{
    // Restore all defaults
    setSettings(QVariantMap{});
}

SimCity4Settings::~SimCity4Settings()
{
}

#include "moc_simcity4settings.cpp"
