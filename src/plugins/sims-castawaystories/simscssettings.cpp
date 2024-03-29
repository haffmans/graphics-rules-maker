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

#include "simscssettings.h"
#include "ui_simscssettings.h"

#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <QMessageBox>

#include <algorithm>

#ifdef Q_OS_WIN32
#include <windows.h>
#include <versionhelpers.h>
#endif

#include "graphicsrulesmaker/devicemodel.h"
#include "graphicsrulesmaker/videocarddatabase.h"

#include "simscsvariables.h"

SimsCSSettings::SimsCSSettings(DeviceModel *devices, VideoCardDatabase *database, QWidget* parent)
    : AbstractSettingsWidget(parent)
    , ui(new Ui::SimsCSSettings)
    , m_devices(devices)
{
    ui->setupUi(this);

    connect(ui->resetDefaults, &QPushButton::clicked, this, &SimsCSSettings::reset);
    connect(ui->autodetect, &QPushButton::clicked, this, &SimsCSSettings::autodetect);

    // Load available resolutions
    QMap<QString, QSize> resolutions;
    QSize previousSize;

    QList<GraphicsMode> validModes = devices->allModes();
    // Remove too low resolutions
    validModes.erase(std::remove_if(validModes.begin(), validModes.end(), [](const GraphicsMode& mode) { return mode.width < 800 || mode.height < 600; }),
                     validModes.end());

    bool have60HzMode = std::any_of(validModes.begin(), validModes.end(), [](const GraphicsMode& mode) { return mode.refreshRate == 60; });
    ui->no60HzAvailable->setVisible(!have60HzMode);

    if (have60HzMode) {
        // Remove all non-60hz modes
        validModes.erase(std::remove_if(validModes.begin(), validModes.end(), [](const GraphicsMode& mode) { return mode.refreshRate != 60; }),
                     validModes.end());
    }

    foreach(const GraphicsMode &mode, validModes) {
        QSize size(mode.width, mode.height);

        if (size != previousSize) {
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

QVariantMap SimsCSSettings::settings() const
{
    SimsCSVariables result;
    result.forceMemory = ui->forceMem->value();
    result.disableTexMemEstimateAdjustment = ui->disableTexMemEstimateAdjustment->isChecked();
    result.enableDriverMemoryManager = ui->enableDriverMemoryManager->isChecked();
    result.disableSimShadows = ui->disableSimShadows->isChecked();
    result.ignoreNvidiaDriverVersion = ui->ignoreNvidiaDriverVersion->isChecked();
    result.radeonHd7000Fix = ui->radeonHd7000Fix->isChecked();
    result.intelHigh = ui->intelHigh->isChecked();
    result.intelVsync = ui->intelVsync->isChecked();
    result.disableDirtyRects = ui->disableDirtyRects->isChecked();

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

void SimsCSSettings::setSettings(const QVariantMap& settings)
{
    SimsCSVariables result(settings);

    ui->forceMem->setValue(result.forceMemory);
    ui->disableTexMemEstimateAdjustment->setChecked(result.disableTexMemEstimateAdjustment);
    ui->enableDriverMemoryManager->setChecked(result.enableDriverMemoryManager);
    ui->disableSimShadows->setChecked(result.disableSimShadows);
    ui->ignoreNvidiaDriverVersion->setChecked(result.ignoreNvidiaDriverVersion);
    ui->radeonHd7000Fix->setChecked(result.radeonHd7000Fix);
    ui->intelHigh->setChecked(result.intelHigh);
    ui->intelVsync->setChecked(result.intelVsync);
    ui->disableDirtyRects->setChecked(result.disableDirtyRects);

    selectResolution(ui->defaultResolution, result.defaultResolution);
    selectResolution(ui->maxResolution, result.maximumResolution);
}

void SimsCSSettings::reset()
{
    setSettings(QVariantMap());
}

void SimsCSSettings::selectResolution(QComboBox* comboBox, const QSize& resolution)
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

void SimsCSSettings::autodetect()
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
        // If the card has 256Mb+ dedicated memory, limit to that (assuming a dedicated graphics card)
        // Otherwise assume an integrated card, and also use its available shared memory (up to 4Gb)
        auto memory = device.memory >= 256*1024*1024
                    ? device.memory
                    : std::min<quint64>(device.memory + device.sharedMemory, 4096ll*1024ll*1024ll);
        maxMemory = std::max<quint64>(maxMemory, memory);
    }
    int maxMemoryMb = static_cast<int>(maxMemory / (1024*1024));
    ui->forceMem->setValue(std::min<int>(ui->forceMem->maximum(), maxMemoryMb));

#ifdef Q_OS_WIN32
    // Use driver memory management on Vista and later
    ui->disableTexMemEstimateAdjustment->setChecked(IsWindowsVistaOrGreater());
    ui->enableDriverMemoryManager->setChecked(IsWindowsVistaOrGreater());
    // Sim Shadows: Windows 8 and up
    ui->disableSimShadows->setChecked(IsWindows8OrGreater() && !shadowFixModInstalled());
#else
    ui->disableTexMemEstimateAdjustment->setChecked(false);
    ui->enableDriverMemoryManager->setChecked(false);
    ui->disableSimShadows->setChecked(false);
#endif

    // Ignore Nvidia driver version: only for Nvidia
    bool hasNvidia = false;
    for (int i = 0; i < deviceCount; ++i) {
        auto device = m_devices->device(i);
        if (device.vendorId == 0x10de) {
            hasNvidia = true;
        }
    }
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
    // - "Iris Graphics", "Iris Plus Graphics", "Iris Pro Graphics", "Iris Xe Graphics"
    // NB: Arc is excluded for now, assuming these tweaks aren't necessary; identification seems to contain "Arc(TM)"
    const QRegularExpression intelHdRegex("U?HD Graphics");
    const QRegularExpression intelIrisRegex("Iris (Plus |Pro |Xe )?Graphics");
    bool hasIntelIntegrated = false;
    for (int i = 0; i < deviceCount; ++i) {
        auto device = m_devices->device(i);
        if (device.vendorId == 0x8086 &&
            (device.name.contains(intelHdRegex) || device.name.contains(intelIrisRegex))) {
            hasIntelIntegrated = true;
            break;
        }
    }
    // Enable V-sync on these modern cards too
    ui->intelVsync->setChecked(hasIntelIntegrated);
    ui->intelHigh->setChecked(hasIntelIntegrated);
    ui->disableDirtyRects->setChecked(hasIntelIntegrated);

    // Resolutions: just pick the last item
    ui->defaultResolution->setCurrentIndex(ui->defaultResolution->count() - 1);
    ui->maxResolution->setCurrentIndex(ui->maxResolution->count() - 1);
}

bool SimsCSSettings::shadowFixModInstalled() const
{
    const QStringList alternatives = {
        "simNopke-simShadowFix0.2.package", "simNopke-simShadowFix0.3.package", "simNopke-simShadowFix0.4.package",
        "simNopke-simShadowFix0.2reallyNotMisty.package", "simNopke-simShadowFix0.3reallyNotMisty.package", "simNopke-simShadowFix0.4reallyNotMisty.package"
    };

    // Fetch folder name from registry (Windows-only)
#ifdef Q_OS_WIN32
#ifdef _WIN64
    QSettings tscsSettings("HKEY_LOCAL_MACHINE\\Software\\WOW6432Node\\Electronic Arts\\The Sims Castaway Stories", QSettings::NativeFormat);
#else
    QSettings tscsSettings("HKEY_LOCAL_MACHINE\\Software\\Electronic Arts\\The Sims Castaway Stories", QSettings::NativeFormat);
#endif
    const auto tscsDisplayName = tscsSettings.value("DisplayName").toString();
#else
    const auto tscsDisplayName = QString("The Sims™ Castaway Stories");
#endif

    if (tscsDisplayName.isEmpty()) {
        return false;
    }

    QString tscsDocumentsDirStr = QStandardPaths::locate(QStandardPaths::DocumentsLocation,
                                                        QString("Electronic Arts") + QDir::separator() + tscsDisplayName,
                                                        QStandardPaths::LocateDirectory);
    if (tscsDocumentsDirStr.isEmpty()) {
        return false; // Game save directory not found
    }

    QDir downloadsDir = QDir(tscsDocumentsDirStr).filePath("Downloads");

    for (const auto& alternative: alternatives) {
        if (downloadsDir.exists(alternative)) {
            return true;
        }
    }
    return false;
}

SimsCSSettings::~SimsCSSettings()
{
}

#include "moc_simscssettings.cpp"
