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

#ifndef SIMS2SETTINGS_H
#define SIMS2SETTINGS_H

#include <QtCore/QVariantMap>
#include <memory>

#include "graphicsrulesmaker/abstractsettingswidget.h"

namespace Ui
{
class SimCity4Settings;
}

class DeviceModel;
class VideoCardDatabase;

class SimCity4Settings : public AbstractSettingsWidget
{
    Q_OBJECT
public:
    SimCity4Settings(DeviceModel* devices, VideoCardDatabase* database, QWidget* parent = 0);
    ~SimCity4Settings();

    QVariantMap settings() const override;
    void setSettings(const QVariantMap & settings) override;

public slots:
    void reset();

private:
    std::unique_ptr<Ui::SimCity4Settings> ui;
};

#endif // SIMS2SETTINGS_H
