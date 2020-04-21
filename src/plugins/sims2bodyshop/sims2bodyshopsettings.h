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

#ifndef SIMS2BODYSHOPSETTINGS_H
#define SIMS2BODYSHOPSETTINGS_H

#include <QtWidgets/QWidget>

namespace Ui
{
class Sims2BodyShopSettings;
}

class DeviceModel;
class VideoCardDatabase;

struct Sims2BodyShopVariables
{
    quint16 forceMemory;
    bool disableTexMemEstimateAdjustment;
    bool enableDriverMemoryManager;
    bool radeonHd7000Fix;
    bool intelHigh;
    bool intelVsync;
};

class Sims2BodyShopSettings : public QWidget
{
    Q_OBJECT
public:
    Sims2BodyShopSettings(DeviceModel* devices, VideoCardDatabase* database, QWidget* parent = 0);
    ~Sims2BodyShopSettings();

    Sims2BodyShopVariables current() const;

public slots:
    void reset();
    void autodetect();

private:
    Ui::Sims2BodyShopSettings* ui;
    DeviceModel *m_devices;
};

#endif // SIMS2BODYSHOPSETTINGS_H