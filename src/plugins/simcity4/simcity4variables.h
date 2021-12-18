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

#ifndef SIMCITY4_VARIABLES_H
#define SIMCITY4_VARIABLES_H

#include <QtCore/QVariantMap>

struct SimCity4Variables
{
    explicit SimCity4Variables(const QVariantMap& map = QVariantMap())
        : radeonHd7000Fix(map.value("radeonHd7000Fix", false).toBool())
        , fastCard(map.value("fastCard", false).toBool())
        , allHighSettings(map.value("allHighSettings", false).toBool())
    {
    }

    operator QVariantMap() const
    {
        return {
            { "radeonHd7000Fix", radeonHd7000Fix },
            { "fastCard", fastCard },
            { "allHighSettings", allHighSettings }
        };
    }

    bool radeonHd7000Fix;
    bool fastCard;
    bool allHighSettings;
};

#endif // SIMCITY4_VARIABLES_H
