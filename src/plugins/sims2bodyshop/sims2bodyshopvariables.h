/*
 * Graphics Rules Maker
 * Copyright (C) 2021-2023 Wouter Haffmans <wouter@simply-life.net>
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

#ifndef SIMS2_BODYSHOP_VARIABLES_H
#define SIMS2_BODYSHOP_VARIABLES_H

#include <QtCore/QVariantMap>

struct Sims2BodyShopVariables
{
    explicit Sims2BodyShopVariables(const QVariantMap& map = QVariantMap())
        : forceMemory(map.value("forceMemory", 0).toUInt())
        , disableTexMemEstimateAdjustment(map.value("disableTexMemEstimateAdjustment", false).toBool())
        , enableDriverMemoryManager(map.value("enableDriverMemoryManager", false).toBool())
        , ignoreNvidiaDriverVersion(map.value("ignoreNvidiaDriverVersion", false).toBool())
        , radeonHd7000Fix(map.value("radeonHd7000Fix", false).toBool())
        , intelHigh(map.value("intelHigh", false).toBool())
        , intelVsync(map.value("intelVsync", false).toBool())
        , disableDirtyRects(map.value("disableDirtyRects", false).toBool())
    {
    }

    operator QVariantMap() const
    {
        return {
            { "forceMemory", forceMemory },
            { "disableTexMemEstimateAdjustment", disableTexMemEstimateAdjustment },
            { "enableDriverMemoryManager", enableDriverMemoryManager },
            { "ignoreNvidiaDriverVersion", ignoreNvidiaDriverVersion },
            { "radeonHd7000Fix", radeonHd7000Fix },
            { "intelHigh", intelHigh },
            { "intelVsync", intelVsync },
            { "disableDirtyRects", disableDirtyRects }
        };
    }

    quint16 forceMemory;
    bool disableTexMemEstimateAdjustment;
    bool enableDriverMemoryManager;
    bool ignoreNvidiaDriverVersion;
    bool radeonHd7000Fix;
    bool intelHigh;
    bool intelVsync;
    bool disableDirtyRects;
};

#endif // SIMS2_BODYSHOP_VARIABLES_H
