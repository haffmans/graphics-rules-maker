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

#ifndef SIMSPSSETTINGS_H
#define SIMSPSSETTINGS_H

#include <graphicsrulesmaker/abstractsettingswidget.h>
#include <memory>

namespace Ui
{
class SimsPSSettings;
}

class QComboBox;
class DeviceModel;
class VideoCardDatabase;

class SimsPSSettings : public AbstractSettingsWidget
{
    Q_OBJECT
public:
    SimsPSSettings(DeviceModel* devices, VideoCardDatabase* database, QWidget* parent = 0);
    ~SimsPSSettings();

    QVariantMap settings() const override;
    void setSettings(const QVariantMap & settings) override;

public slots:
    void reset();
    void autodetect();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    void selectResolution(QComboBox* comboBox, const QSize& resolution);
    bool shadowFixModInstalled() const;
    void resizeRefreshRateWarning();
    std::unique_ptr<Ui::SimsPSSettings> ui;
    DeviceModel *m_devices;
};

#endif // SIMSPSSETTINGS_H
