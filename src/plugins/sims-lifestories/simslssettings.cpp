/*
 * Graphics Rules Maker
 * Copyright (C) 2014-2018 Wouter Haffmans <wouter@simply-life.net>
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

#include "simslssettings.h"
#include "ui_simslssettings.h"

#include <QtCore/QSettings>
#include <QMessageBox>

#ifdef Q_OS_WIN32
#include <windows.h>
#include <versionhelpers.h>
#endif

#include "graphicsrulesmaker/devicemodel.h"
#include "graphicsrulesmaker/videocarddatabase.h"

SimsLSSettings::SimsLSSettings(DeviceModel *devices, VideoCardDatabase *database, QWidget* parent)
    : QWidget(parent)
    , m_devices(devices)
{
    ui = new Ui::SimsLSSettings;
    ui->setupUi(this);

    connect(ui->resetDefaults, &QPushButton::clicked, this, &SimsLSSettings::reset);
    connect(ui->autodetect, &QPushButton::clicked, this, &SimsLSSettings::autodetect);

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

    ui->defaultResolution->addItems(resolutions);
    ui->maxResolution->addItems(resolutions);

    // Load Settings
    QSettings s;
    s.beginGroup("sims-lifestories");

    ui->forceMem->setValue(s.value("forceMemory", 0).toInt());
    ui->disableTexMemEstimateAdjustment->setChecked(s.value("disableTexMemEstimateAdjustment", false).toBool());
    ui->disableSimShadows->setChecked(s.value("disableSimShadows", false).toBool());
    ui->radeonHd7000Fix->setChecked(s.value("radeonHd7000Fix", false).toBool());
    ui->intelHigh->setChecked(s.value("intelHigh", false).toBool());
    ui->intelVsync->setChecked(s.value("intelVsync", false).toBool());
    ui->defaultResolution->setCurrentText(s.value("defaultResolution", "1024x768").toString());
    ui->maxResolution->setCurrentText(s.value("maximumResolution", "1600x1200").toString());

    s.endGroup();
}

SimsLSVariables SimsLSSettings::current() const
{
    QRegExp resolutionString("^(\\d+)x(\\d+)$");
    SimsLSVariables result;
    result.forceMemory = ui->forceMem->value();
    result.disableTexMemEstimateAdjustment = ui->disableTexMemEstimateAdjustment->isChecked();
    result.disableSimShadows = ui->disableSimShadows->isChecked();
    result.radeonHd7000Fix = ui->radeonHd7000Fix->isChecked();
    result.intelHigh = ui->intelHigh->isChecked();
    result.intelVsync = ui->intelVsync->isChecked();

    if (resolutionString.exactMatch(ui->defaultResolution->currentText())) {
        result.defaultResolution.setWidth(resolutionString.cap(1).toInt());
        result.defaultResolution.setHeight(resolutionString.cap(2).toInt());
    }

    if (resolutionString.exactMatch(ui->maxResolution->currentText())) {
        result.maximumResolution.setWidth(resolutionString.cap(1).toInt());
        result.maximumResolution.setHeight(resolutionString.cap(2).toInt());
    }

    return result;
}

void SimsLSSettings::autodetect()
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

    // Sim Shadows: Windows 8 and up
#ifdef Q_OS_WIN32
    ui->disableSimShadows->setChecked(IsWindows8OrGreater());
#else
    ui->disableSimShadows->setChecked(false);
#endif

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

    // Resolutions: just pick the last item
    ui->defaultResolution->setCurrentIndex(ui->defaultResolution->count() - 1);
    ui->maxResolution->setCurrentIndex(ui->maxResolution->count() - 1);
}

void SimsLSSettings::reset()
{
    // Restore all defaults
    ui->forceMem->setValue(0);
    ui->disableTexMemEstimateAdjustment->setChecked(false);
    ui->disableSimShadows->setChecked(false);
    ui->radeonHd7000Fix->setChecked(false);
    ui->intelHigh->setChecked(false);
    ui->intelVsync->setChecked(false);
    ui->defaultResolution->setCurrentText("1024x768");
    ui->maxResolution->setCurrentText("1600x1200");
}

SimsLSSettings::~SimsLSSettings()
{
    // Write settings
    QSettings s;
    SimsLSVariables vars = current();

    s.beginGroup("sims-lifestories");
    s.setValue("forceMemory", ui->forceMem->value());
    s.setValue("disableTexMemEstimateAdjustment", ui->disableTexMemEstimateAdjustment->isChecked());
    s.setValue("disableSimShadows", ui->disableSimShadows->isChecked());
    s.setValue("radeonHd7000Fix", ui->radeonHd7000Fix->isChecked());
    s.setValue("intelHigh", ui->intelHigh->isChecked());
    s.setValue("intelVsync", ui->intelVsync->isChecked());
    s.setValue("defaultResolution", ui->defaultResolution->currentText());
    s.setValue("maximumResolution", ui->maxResolution->currentText());
    s.endGroup();

    delete ui;
}

#include "moc_simslssettings.cpp"
