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

#include "simspssettings.h"
#include "ui_simspssettings.h"

#include <QtCore/QSettings>
#include <QMessageBox>

#ifdef Q_OS_WIN32
#include <windows.h>
#include <versionhelpers.h>
#endif

#include "graphicsrulesmaker/devicemodel.h"
#include "graphicsrulesmaker/videocarddatabase.h"

#include "simspsvariables.h"

SimsPSSettings::SimsPSSettings(DeviceModel *devices, VideoCardDatabase *database, QWidget* parent)
    : AbstractSettingsWidget(parent)
    , ui(new Ui::SimsPSSettings)
    , m_devices(devices)
{
    ui->setupUi(this);

    connect(ui->resetDefaults, &QPushButton::clicked, this, &SimsPSSettings::reset);
    connect(ui->autodetect, &QPushButton::clicked, this, &SimsPSSettings::autodetect);

    // Load available resolutions
    QMap<QString, QSize> resolutions;
    QSize previousSize;

    foreach(const GraphicsMode &mode, devices->allModes()) {
        // We don't care about refresh rates - i.e. avoid duplicates from the list. Also ignore anything below 800x600.
        QSize size(mode.width, mode.height);
        if (size != previousSize && size.width() >= 800 && size.height() >= 600) {
            previousSize = size;

            QString text = QString("%1x%2").arg(size.width()).arg(size.height());
            ui->defaultResolution->addItem(text, size);
            ui->maxResolution->addItem(text, size);
        }
    }

    // Pick the best matching default resolutions
    selectResolution(ui->defaultResolution, QSize(1024, 768));
    selectResolution(ui->maxResolution, QSize(1600, 1200));
}

QVariantMap SimsPSSettings::settings() const
{
    SimsPSVariables result;
    result.forceMemory = ui->forceMem->value();
    result.disableTexMemEstimateAdjustment = ui->disableTexMemEstimateAdjustment->isChecked();
    result.enableDriverMemoryManager = ui->enableDriverMemoryManager->isChecked();
    result.disableSimShadows = ui->disableSimShadows->isChecked();
    result.radeonHd7000Fix = ui->radeonHd7000Fix->isChecked();
    result.intelHigh = ui->intelHigh->isChecked();
    result.intelVsync = ui->intelVsync->isChecked();

    auto defaultResolution = ui->defaultResolution->currentData().toSize();
    if (defaultResolution.isValid()) {
        result.defaultResolution = defaultResolution;
    }

    auto maxResolution = ui->maxResolution->currentData().toSize();
    if (maxResolution.isValid()) {
        result.maximumResolution = maxResolution;
    }

    return result;
}

void SimsPSSettings::setSettings(const QVariantMap& settings)
{
    SimsPSVariables result(settings);

    ui->forceMem->setValue(result.forceMemory);
    ui->disableTexMemEstimateAdjustment->setChecked(result.disableTexMemEstimateAdjustment);
    ui->enableDriverMemoryManager->setChecked(result.enableDriverMemoryManager);
    ui->disableSimShadows->setChecked(result.disableSimShadows);
    ui->radeonHd7000Fix->setChecked(result.radeonHd7000Fix);
    ui->intelHigh->setChecked(result.intelHigh);
    ui->intelVsync->setChecked(result.intelVsync);

    selectResolution(ui->defaultResolution, result.defaultResolution);
    selectResolution(ui->maxResolution, result.maximumResolution);
}

void SimsPSSettings::reset()
{
    setSettings(QVariantMap());
}

void SimsPSSettings::selectResolution(QComboBox* comboBox, const QSize& resolution)
{
    QSize bestMatch;
    int bestMatchIndex = -1;

    for (int i = 0; i < comboBox->count(); ++i) {
        auto itemSize = comboBox->itemData(i).toSize();

        // If this comes closer to the requested resolution (in both dimensions), it's a better match
        if ((bestMatch.width() <= itemSize.width() && itemSize.width() <= resolution.width()) &&
            (bestMatch.height() <= itemSize.height() && itemSize.height() <= resolution.height())) {
            bestMatch = itemSize;
            bestMatchIndex = i;
        }
    }

    if (bestMatchIndex >= 0) {
        comboBox->setCurrentIndex(bestMatchIndex);
    }
}

void SimsPSSettings::autodetect()
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

SimsPSSettings::~SimsPSSettings()
{
}

#include "moc_simspssettings.cpp"
