/*
 * Graphics Rules Maker
 * Copyright (C) 2014-2023 Wouter Haffmans <wouter@simply-life.net>
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

#include "sims2bodyshopsettings.h"
#include "ui_sims2bodyshopsettings.h"

#include <QtCore/QSettings>
#include <QMessageBox>

#ifdef Q_OS_WIN32
#include <windows.h>
#include <versionhelpers.h>
#endif

#include "graphicsrulesmaker/devicemodel.h"
#include "graphicsrulesmaker/videocarddatabase.h"

#include "sims2bodyshopvariables.h"

Sims2BodyShopSettings::Sims2BodyShopSettings(DeviceModel *devices, VideoCardDatabase *database, QWidget* parent)
    : AbstractSettingsWidget(parent)
    , ui(new Ui::Sims2BodyShopSettings)
    , m_devices(devices)
{
    ui->setupUi(this);

    connect(ui->resetDefaults, &QPushButton::clicked, this, &Sims2BodyShopSettings::reset);
    connect(ui->autodetect, &QPushButton::clicked, this, &Sims2BodyShopSettings::autodetect);
}

QVariantMap Sims2BodyShopSettings::settings() const
{
    Sims2BodyShopVariables result;
    result.forceMemory = ui->forceMem->value();
    result.disableTexMemEstimateAdjustment = ui->disableTexMemEstimateAdjustment->isChecked();
    result.enableDriverMemoryManager = ui->enableDriverMemoryManager->isChecked();
    result.ignoreNvidiaDriverVersion = ui->ignoreNvidiaDriverVersion->isChecked();
    result.radeonHd7000Fix = ui->radeonHd7000Fix->isChecked();
    result.intelHigh = ui->intelHigh->isChecked();
    result.intelVsync = ui->intelVsync->isChecked();

    return result;
}

void Sims2BodyShopSettings::setSettings(const QVariantMap& settings)
{
    Sims2BodyShopVariables result(settings);

    ui->forceMem->setValue(result.forceMemory);
    ui->disableTexMemEstimateAdjustment->setChecked(result.disableTexMemEstimateAdjustment);
    ui->enableDriverMemoryManager->setChecked(result.enableDriverMemoryManager);
    ui->ignoreNvidiaDriverVersion->setChecked(result.ignoreNvidiaDriverVersion);
    ui->radeonHd7000Fix->setChecked(result.radeonHd7000Fix);
    ui->intelHigh->setChecked(result.intelHigh);
    ui->intelVsync->setChecked(result.intelVsync);
}

void Sims2BodyShopSettings::reset()
{
    setSettings(QVariantMap());
}

void Sims2BodyShopSettings::autodetect()
{
    auto result = QMessageBox::information(this, tr("Auto-detect Settings"),
        tr("Graphics Rules Maker will now attempt to automatically detect the best settings for your system. They will not be saved until you click \"Save Files\".\n\nThese settings are not guaranteed to work!\n\nIf the detected settings do not help, you should change the options manually.\n\nDo you want to continue?"),
        QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);

    if (result != QMessageBox::Ok) {
        return;
    }

    int deviceCount = m_devices->rowCount();

    // Force memory
    quint64 maxMemory = 0;
    for (int i = 0; i < deviceCount; ++i) {
        auto device = m_devices->device(i);
        maxMemory = std::max<quint64>(maxMemory, device.memory);
    }
    int maxMemoryMb = static_cast<int>(maxMemory / (1024*1024));
    ui->forceMem->setValue(std::min<int>(ui->forceMem->maximum(), maxMemoryMb));

    // Disabling texture memory estimate adjustment: nVidia cards only for now
    bool hasNvidia = false;
    for (int i = 0; i < deviceCount; ++i) {
        auto device = m_devices->device(i);
        if (device.vendorId == 0x10de) {
            hasNvidia = true;
        }
    }

    ui->disableTexMemEstimateAdjustment->setChecked(hasNvidia);
#ifdef Q_OS_WIN32
    ui->enableDriverMemoryManager->setChecked(IsWindowsVistaOrGreater()); // turn on by default for Vista and later
#else
    ui->enableDriverMemoryManager->setChecked(false);
#endif

    // Ignore Nvidia driver version: only for Nvidia
    ui->ignoreNvidiaDriverVersion->setChecked(hasNvidia);

    // Radeon HD 7000 tweak
    bool applyRadeonTweak = false;
    const QRegularExpression radeon7000HdRegex("Radeon.*HD.*7\\d00");
    for (int i = 0; i < deviceCount; ++i) {
        auto device = m_devices->device(i);
        if (device.name.contains(radeon7000HdRegex)) {
            applyRadeonTweak = true;
            break;
        }
    }
    ui->radeonHd7000Fix->setChecked(applyRadeonTweak);

    // Intel High mode enabled for these series:
    // - "HD Graphics", "UHD Graphics",
    // - "Iris Graphics", "Iris Plus Graphics", "Iris Pro Graphics"
    const QRegularExpression intelHdRegex("U?HD Graphics");
    const QRegularExpression intelIrisRegex("Iris (Plus |Pro )?Graphics");
    bool intelHigh = false;
    for (int i = 0; i < deviceCount; ++i) {
        auto device = m_devices->device(i);
        if (device.name.contains(intelHdRegex) || device.name.contains(intelIrisRegex)) {
            intelHigh = true;
            break;
        }
    }
    // Enable V-sync on these modern cards too
    ui->intelVsync->setChecked(intelHigh);
    ui->intelHigh->setChecked(intelHigh);
}

Sims2BodyShopSettings::~Sims2BodyShopSettings()
{
}

#include "moc_sims2bodyshopsettings.cpp"
