/*
 * Graphics Rules Maker
 * Copyright (C) 2021 Wouter Haffmans <wouter@simply-life.net>
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

#ifndef SIMSCS_VARIABLES_H
#define SIMSCS_VARIABLES_H

#include <QtCore/QVariantMap>
#include <QtCore/QSize>
#include <QtCore/QRegularExpression>

struct SimsCSVariables
{
    explicit SimsCSVariables(const QVariantMap& map = QVariantMap())
        : forceMemory(map.value("forceMemory", 0).toUInt())
        , disableTexMemEstimateAdjustment(map.value("disableTexMemEstimateAdjustment", false).toBool())
        , enableDriverMemoryManager(map.value("enableDriverMemoryManager", false).toBool())
        , disableSimShadows(map.value("disableSimShadows", false).toBool())
        , ignoreNvidiaDriverVersion(map.value("ignoreNvidiaDriverVersion", false).toBool())
        , radeonHd7000Fix(map.value("radeonHd7000Fix", false).toBool())
        , intelHigh(map.value("intelHigh", false).toBool())
        , intelVsync(map.value("intelVsync", false).toBool())
        , disableDirtyRects(map.value("disableDirtyRects", false).toBool())
        , defaultResolution(map.value("defaultResolution", QSize(1024, 768)).toSize())
        , maximumResolution(map.value("maximumResolution", QSize(1600, 1200)).toSize())
    {
        const QRegularExpression resolutionRegex("^(\\d+)x(\\d+)$");

        // Backwards compatibility: support loading resolution strings
        if (map.contains("defaultResolution") && map.value("defaultResolution").typeId() == QMetaType::QString) {
            auto stringResolution = map.value("defaultResolution").toString();
            auto match = resolutionRegex.match(stringResolution);
            if (match.hasMatch()) {
                defaultResolution.setWidth(match.capturedView(1).toInt());
                defaultResolution.setHeight(match.capturedView(2).toInt());
            }
        }

        if (map.contains("maximumResolution") && map.value("maximumResolution").typeId() == QMetaType::QString) {
            auto stringResolution = map.value("maximumResolution").toString();
            auto match = resolutionRegex.match(stringResolution);
            if (match.hasMatch()) {
                maximumResolution.setWidth(match.capturedView(1).toInt());
                maximumResolution.setHeight(match.capturedView(2).toInt());
            }
        }
    }

    operator QVariantMap() const
    {
        return {
            { "forceMemory", forceMemory },
            { "disableTexMemEstimateAdjustment", disableTexMemEstimateAdjustment },
            { "enableDriverMemoryManager", enableDriverMemoryManager },
            { "disableSimShadows", disableSimShadows },
            { "ignoreNvidiaDriverVersion", ignoreNvidiaDriverVersion },
            { "radeonHd7000Fix", radeonHd7000Fix },
            { "intelHigh", intelHigh },
            { "intelVsync", intelVsync },
            { "disableDirtyRects", disableDirtyRects },
            { "defaultResolution", defaultResolution },
            { "maximumResolution", maximumResolution }
        };
    }

    quint16 forceMemory;
    bool disableTexMemEstimateAdjustment;
    bool enableDriverMemoryManager;
    bool disableSimShadows;
    bool ignoreNvidiaDriverVersion;
    bool radeonHd7000Fix;
    bool intelHigh;
    bool intelVsync;
    bool disableDirtyRects;

    QSize defaultResolution;
    QSize maximumResolution;
};

#endif // SIMSCS_VARIABLES_H
