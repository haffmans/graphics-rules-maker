/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  W. Haffmans <wouter@simply-life.net>
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

#include "sims2settings.h"
#include "ui_sims2settings.h"

#include <QtCore/QSettings>

Sims2Settings::Sims2Settings(QWidget* parent)
{
    ui = new Ui::Sims2Settings;
    ui->setupUi(this);

    // Load available resolutions

    // Load Settings
    QSettings s;
    s.beginGroup("sims2");

    ui->forceMem->setValue(s.value("forceMemory", 0).toInt());
    ui->disableSimShadows->setChecked(s.value("disableSimShadows", false).toBool());
    ui->defaultResolution->setCurrentText(s.value("defaultResolution", "1024x768").toString());
    ui->maxResolution->setCurrentText(s.value("maximumResolution", "1600x1200").toString());

    s.endGroup();
}

Sims2Variables Sims2Settings::current() const
{
    QRegExp resolutionString("^(\\d+)x(\\d+)$");
    Sims2Variables result;
    result.forceMemory = ui->forceMem->value();
    result.disableSimShadows = ui->disableSimShadows->isChecked();

    if (resolutionString.exactMatch(ui->defaultResolution->currentText())) {
        result.defaultResolution.setWidth(resolutionString.cap(1).toInt());
        result.defaultResolution.setHeight(resolutionString.cap(2).toInt());
    }

    if (resolutionString.exactMatch(ui->maxResolution->currentText())) {
        result.maximumResolution.setWidth(resolutionString.cap(1).toInt());
        result.maximumResolution.setHeight(resolutionString.cap(2).toInt());
    }
}


Sims2Settings::~Sims2Settings()
{
    // Write settings
    QSettings s;
    Sims2Variables vars = current();

    s.beginGroup("sims2");
    s.setValue("forceMemory", ui->forceMem->value());
    s.setValue("disableSimShadows", ui->disableSimShadows->isChecked());
    s.setValue("defaultResolution", ui->defaultResolution->currentText());
    s.setValue("maximumResolution", ui->maxResolution->currentText());
    s.endGroup();

    delete ui;
}
