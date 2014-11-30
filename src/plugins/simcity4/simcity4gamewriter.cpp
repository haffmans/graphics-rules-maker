/*
 * Graphics Rules Maker
 * Copyright (C) 2014 Wouter Haffmans <wouter@simply-life.net>
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

#include "simcity4gamewriter.h"

#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>

#include "simcity4settings.h"

SimCity4GameWriter::SimCity4GameWriter(QObject* parent)
    : QObject(parent), GameWriterInterface()
{
}

QWidget* SimCity4GameWriter::settingsWidget(DeviceModel* devices, VideoCardDatabase* database, QWidget* parent)
{
    return new SimCity4Settings(devices, database, parent);
}

QDir SimCity4GameWriter::findGameDirectory() const
{
#ifdef Q_OS_WIN32
    // This really only works on Windows...
    // Order based on "Suppression Exe" contents
    QStringList exes = QStringList() << "SimCity 4.exe";

    QString result;
    QSettings s("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\App Paths", QSettings::NativeFormat);
    foreach(const QString &exe, exes) {
        // Find path
        s.beginGroup(exe);
        QFileInfo file(s.value("Default").toString());
        if (file.exists()) {
            // Item found and file exists -- use "Path" setting
            result = s.value("Path").toString();
            break;
        }
        s.endGroup();
    }

    return QDir(result);
#else
    // TODO Implement this stuff for Mac?
    return QDir();
#endif
}

QFileInfo SimCity4GameWriter::gameExecutable(const QDir& gameDirectory) const
{
    return findFile(gameDirectory, "Apps/SimCity 4.exe");
}

QFileInfo SimCity4GameWriter::rulesFileName(const QDir& gameDirectory) const
{
    return findFile(gameDirectory, "Graphics Rules.sgr");
}

QFileInfo SimCity4GameWriter::databaseFileName(const QDir& gameDirectory) const
{
    return findFile(gameDirectory, "Video Cards.sgr");
}

QFileInfo SimCity4GameWriter::findFile(QDir baseDir, const QString& file) const
{
    QString path = baseDir.absoluteFilePath(file);
    if (QFileInfo::exists(path)) {
        return path;
    }

    return QFileInfo();
}


void SimCity4GameWriter::write(QWidget* settingsWidget, QIODevice* target)
{
    // Load settings
    SimCity4Settings *widget = qobject_cast<SimCity4Settings*>(settingsWidget);
    if (!widget) {
        return;
    }

    SimCity4Variables options = widget->current();

    // Write code - Lots of ugly code follows, beware....
    QTextStream stream(target);
    stream << R"EOF(#############################################################################
#
# property groups for options set via the UI
#


set VisualEffects             0
set CarsSims                  1
set Shadows                   2
set TextureQuality            3
set CloudsFog                 4
set Waves                     5
set Driver                    6
set Cursor                    7
set Translucency              8
set CityDetail                9
set VariableSpeedAutomata    10

set ScreenSize               11
set ScreenDepth              12
set DayNight                 13

set BuildingVarietyGroup     14
set BuildingLoaderSpeedGroup 15

#############################################################################

set Off 0
set On  1

set Low     1
set Medium  2
set High    3

set CursorBW    0
set CursorColor 1

set Software    0
set Hardware    1

set Screen_800x600    0
set Screen_1024x768   1
set Screen_1280x1024  2
set Screen_1600x1200  3

set Depth_16    0
set Depth_32    1

#############################################################################

optionGroup $VisualEffects
   option $Low
      property particleDensity       0.25
      property particleScale         2.0
      property maxParticlesTarget    400
      property particleDamping       0.0
      property particleLODOffset     2    # subtracted from the zoom.
      property effectPriorityLevel   1
      property useScreenShake        false
      property useScreenFlash        false

      property demolishModelThreshold 24

   option $Medium
      property particleDensity       0.5
      property particleScale         2.0
      property maxParticlesTarget    1200
      property particleDamping       0.0
      property particleLODOffset     0    # subtracted from the zoom.
      property effectPriorityLevel   3
      property useScreenShake        true
      property useScreenFlash        true

      property demolishModelThreshold 18

   option $High
      property particleDensity       1
      property particleScale         1
      property maxParticlesTarget    5000
      property particleDamping       0.0
      property particleLODOffset     0    # subtracted from the zoom.
      property effectPriorityLevel   5
      property useScreenShake        true
      property useScreenFlash        true

      property demolishModelThreshold 12
end

optionGroup $CarsSims
# Sadly, we have no off
#  option $Off
   option $Low
      property maxVehiclePct    0.0
      property maxPedPct        0.0
      property minVehicleZoom   5
      property minPedZoom       5

#   option $Low
#      property maxVehiclePct    0.2
#      property maxPedPct        0.2
#      property minVehicleZoom   5
#      property minPedZoom       5

   option $Medium
      property maxVehiclePct    0.5
      property maxPedPct        0.5
      property minVehicleZoom   4
      property minPedZoom       5

   option $High
      property maxVehiclePct    1
      property maxPedPct        1
      property minVehicleZoom   3
      property minPedZoom       4
end

optionGroup $Shadows
# Sadly, we have no off
#      option $Off
      option $Low
      property shadowQuality 0 # off
      property shadowModelThreshold 40
      property lightingUpdateDelta 0.2

#   option $Low
#      property shadowQuality 2
#      property shadowModelThreshold 40
#      property lightingUpdateDelta 0.2

   option $Medium
      property shadowQuality 3
      property shadowModelThreshold 22
      property lightingUpdateDelta 0.05

   option $High
      property shadowQuality 5
      property shadowModelThreshold 12
      property lightingUpdateDelta 1e-4
end


optionGroup $CloudsFog
   option $On
      property renderCloudsAndFog true
   option $Off
      property renderCloudsAndFog false
end

optionGroup $Waves
   option $On
      property renderWaterEffects true

   option $Off
      property renderWaterEffects false
end


optionGroup $Cursor
   option $CursorBW
      property cursorType 1

   option $CursorColor
      property cursorType 3 # 256-color
end

optionGroup $Translucency
   option $On
      property preferOpaque false

   option $Off
      property preferOpaque true
end

optionGroup $CityDetail
   option $Low
      property renderModelThreshold 12
   option $Medium
      property renderModelThreshold 6
   option $High
      property renderModelThreshold 2
end

optionGroup $BuildingVarietyGroup
   option $Low
      property buildingVariety 0
   option $Medium
      property buildingVariety 1
   option $High
      property buildingVariety 2
end

optionGroup $BuildingLoaderSpeedGroup
   option $Low
      property buildingLoaderSpeed 0
   option $Medium
      property buildingLoaderSpeed 1
   option $High
      property buildingLoaderSpeed 2
end

optionGroup $TextureQuality
   option $Low
      property modelLODOffset 2
   option $Medium
      property modelLODOffset 1
   option $High
      property modelLODOffset 0
end


#############################################################################
#
# Configuration Rules
#

#<

   Current configuration sources, with example values:

   Flags (has, hasNo):
      hardwareDriver = true
      DXT = true
      fullscreen = false
      MMX = true
      FPU = true
      backingStore = true

   Strings (stringMatch, stringNotMatch):
      userName = awillmott
      appName = SimCity 4
      version = 1.0.238.0
      osVersion = Windows NT 5.0
      computerName = AWILLMOTT-6776
      CPU = GenuineIntel
      sglDriverName = DirectX
      sglDriverVersion = 2.0
      driverName = nv4_disp.dll
      driverVersion = 6.13.10.3082, GUID: D7B71E3E-4110-11CF-F576-37200CC2CD35
      cardName = NVIDIA GeForce4 Ti 4600
      cardVersion = Vendor: 10de, Device: 0250, Board: 371545, Chipset: 00a3
      cardIdentity = NVIDIA GeForce4 Ti 4600
      soundDriverName = Unknown
      soundCardName = Unknown
      buildType = Beta

   Numbers (atLeast, atMost):
      cpuSpeed = 1894
      memory = 1024
      freeMemory = 624
      screenWidth = 1600
      screenHeight = 1200
      screenBPP = 16
      videoMemory = 123
      textureMemory = 123
      textureStages = 4

   Processing stops after a rule if the rule matches.
   Partial rules continue processing.
   A rule with -any is accepted if any of its conditions are
   met -- the default is that all conditions must be met.
#>



# Sound
partialRule "Sound"
   rule "high sound detail"
      atLeast cpuSpeed 1500
      property soundDetail 2
   end
   rule "medium sound detail"
      atLeast cpuSpeed 800
      property soundDetail 1
   end
   rule "low sound detail"
      property soundDetail 0
   end
end


partialRule "City Detail and Day/Night Settings"
   rule
      # Windows XP
      stringMatch osVersion "Windows NT 5.1"

      rule "high computer power"
         atLeast memory                      800
         atLeast cpuSpeed                    2400
         option  $CityDetail                 $High
         option  $DayNight                   $On
         option  $BuildingLoaderSpeedGroup   $High
      end
      rule "medium computer power"
         atLeast memory                       600
         atLeast cpuSpeed                     1800
         option $CityDetail                  $Medium
         option $DayNight                    $Off
         option $BuildingLoaderSpeedGroup    $Medium
      end
      rule "low computer power"
         option $CityDetail                  $Low
         option $DayNight                    $Off
         option $BuildingLoaderSpeedGroup    $Low
      end
   end
   rule
      # Windows 2000
      stringMatch osVersion "Windows NT 5.0"

      rule "high computer power"
         atLeast memory                      700
         atLeast cpuSpeed                    2000
         option  $CityDetail                 $High
         option  $DayNight                   $On
         option  $BuildingLoaderSpeedGroup   $High
      end
      rule "medium computer power"
         atLeast memory                       500
         atLeast cpuSpeed                     1400
         option $CityDetail                  $Medium
         option $DayNight                    $Off
         option $BuildingLoaderSpeedGroup    $Medium
      end
      rule "low computer power"
         option $CityDetail                  $Low
         option $DayNight                    $Off
         option $BuildingLoaderSpeedGroup    $Low
      end
   end
   rule
      # Windows 98, or anything beyond Windows XP.

      rule "high computer power"
         atLeast memory                      500
         atLeast cpuSpeed                    1800
         option  $CityDetail                 $High
         option  $DayNight                   $On
         option  $BuildingLoaderSpeedGroup   $High
      end
      rule "medium computer power"
         atLeast memory                       400
         atLeast cpuSpeed                     1600
         option $CityDetail                  $Medium
         option $DayNight                    $Off
         option $BuildingLoaderSpeedGroup    $Medium
      end
      rule "low computer power"
         option $CityDetail                  $Low
         option $DayNight                    $Off
         option $BuildingLoaderSpeedGroup    $Low
      end
   end
end

partialRule "Texture Quality and Building Variety Settings"
   rule
      # Windows XP
      stringMatch osVersion "Windows NT 5.1"

      rule "high memory"
         atLeast memory                500
         option $TextureQuality        $High
         option $BuildingVarietyGroup  $High
      end
      rule "medium memory"
         atLeast memory                 300
         option $TextureQuality        $Medium
         option $BuildingVarietyGroup  $Medium
      end
      rule "low memory"
         option $TextureQuality        $Low
         option $BuildingVarietyGroup  $Low
      end
   end
   rule
      # Windows 2000
      stringMatch osVersion "Windows NT 5.0"

      rule "high memory"
         atLeast memory                500
         option $TextureQuality        $High
         option $BuildingVarietyGroup  $High
      end
      rule "medium memory"
         atLeast memory                 200
         option $TextureQuality        $Medium
         option $BuildingVarietyGroup  $Medium
      end
      rule "low memory"
         option $TextureQuality        $Low
         option $BuildingVarietyGroup  $Low
      end
   end
   rule
      # Windows 98, or anything beyond Windows XP.

      rule "high memory"
         atLeast memory                500
         option $TextureQuality        $High
         option $BuildingVarietyGroup  $High
      end
      rule "medium memory"
         atLeast memory                 200
         option $TextureQuality        $Medium
         option $BuildingVarietyGroup  $Medium
      end
      rule "low memory"
         option $TextureQuality        $Low
         option $BuildingVarietyGroup  $Low
      end
   end
end

partialRule "Variable Speed Automata"
   rule "default"
      option $VariableSpeedAutomata $On
   end
end

partialRule "Recommend Software" -any
   stringMatch cardIdentity "3D Labs*"
   stringMatch cardIdentity "*Kyro*"

   partialRule "Non-shared memory"
      stringNotMatch cardIdentity "Intel*"
      atMost videoMemory 15 # less than recommended
   end

   property forceSoftwareDriver $prefsFirstTime
end


# must come before software driver rules, after "Recommend Software".
partialRule "No backing store"
   hasNo backingStore

   property forceSoftwareDriver true   # need to force this immediately
end


rule "Standard software"
   hasNo hardwareDriver

   option $VisualEffects   $Low
   option $CarsSims        $Low
   option $Shadows         $Low
   option $CloudsFog       $Off
   option $Waves           $Off
   option $Translucency    $On
   option $Cursor          $CursorBW
   option $Driver          $Software
   option $ScreenSize      $Screen_800x600
   option $ScreenDepth     $Depth_16

   partialRule
      # Enable more visual effects if user has decent computing power.
      atLeast memory          128
      atLeast cpuSpeed       1000
      option $VisualEffects $Medium
   end

   partialRule
      # Enable clouds and fog if user has decent computing power.
      atLeast memory       256
      atLeast cpuSpeed    2400
      option $CloudsFog  $On
   end

   property useSecondStage      false
   property texBindMaxFree       4   # Software now caches DXT.
   property dirtyRectMergeFrames 1   # software is pretty good about BS updates
   property TextureBits         32   # software hardwired for 32-bit internally.
end


rule "Standard hardware"
)EOF";
    if (options.allHighSettings) {
        stream << R"EOF(
   # GraphicsRulesMaker Tweak: Force high quality settings
   option $VisualEffects   $High
   option $CarsSims        $High
   option $Shadows         $High
   option $CloudsFog       $On
   option $Waves           $On
   option $Translucency    $On
   option $Cursor          $CursorColor
   option $Driver          $Hardware
   option $ScreenSize      $Screen_1280x1024  # 1600x1200 may not work on HD screens due to height (1920x1080)
   option $ScreenDepth     $Depth_32

   property useSecondStage       true  # Default, overridden below in most cases.
   property texBindMaxFree       8   # To stop thrashing, plus a little pad.
   property dirtyRectMergeFrames 2   # Default, for low-end card.
   property TextureBits          32  # mostly just for preview -- everything else
                                     # should be DXT
)EOF";
    }
    else {
        // Default HW rules
        stream << R"EOF(
   option $VisualEffects   $Low
   option $CarsSims        $Low
   option $Shadows         $Low
   option $CloudsFog       $Off
   option $Waves           $On
   option $Translucency    $On
   option $Cursor          $CursorColor
   option $Driver          $Hardware
   option $ScreenSize      $Screen_800x600
   option $ScreenDepth     $Depth_16

   property useSecondStage       false  # Default, overridden below in most cases.
   property texBindMaxFree       4   # To stop thrashing, plus a little pad.
   property dirtyRectMergeFrames 6   # Default, for low-end card.
   property TextureBits          32  # mostly just for preview -- everything else
                                     # should be DXT

   partialRule
      # Enable more cars and sims if user has decent computing power.
      atLeast memory          256
      atLeast cpuSpeed       1600
      option $CarsSims      $Medium
   end

   partialRule
      # Enable more visual effects if user has decent computing power.
      atLeast memory          128
      atLeast cpuSpeed        700
      option $VisualEffects $Medium
   end

   partialRule
      # Enable more shadows if user has decent computing power.
      atLeast memory          256
      atLeast cpuSpeed       1800
      option $Shadows       $Medium
   end

   partialRule
      # Enable clouds and fog if user has decent computing power.
      atLeast memory          300
      atLeast cpuSpeed       1600
      option $CloudsFog     $On
   end

   partialRule
      # Enable higher screen resolution if user has decent computing power.
      atLeast memory          256
      atLeast cpuSpeed       1600
      option $ScreenSize     $Screen_1024x768
   end

   partialRule "No DXT"
      hasNo DXT

      property TextureBits 16  # we'll need all the VRAM we can get...
      option $ScreenDepth  $Depth_16
   end

   partialRule "Two stages"
      atLeast textureStages 2

      property useSecondStage       true
      property dirtyRectMergeFrames 4
   end

   partialRule "Lotsa vidmem"
      atLeast videoMemory 100  # includes AGP

      property texBindMaxFree  8 # keep a little more round
   end

   partialRule "Slow card"
      stringMatch cardIdentity "ATI*128*"
      stringMatch cardIdentity "ATI*Rage*"
      stringMatch cardIdentity "ATI*Mach*"

      option $Shadows    $Low
   end

   # The Radeon series has a very slow path for partial
   # depth buffer copies. This has apparently been
   # fixed for 9500 and later models in the latest drivers,
   # but cannot be fixed on previous Radeons because of
   # hardware limitations.
   # NOTE: disabling this rule unnecessarily may lead to
   # very poor graphics performance.
   partialRule "Radeon"
      stringMatch cardIdentity "*Radeon*"

      partialRule "SlowSubrectDepthCopy" -any
)EOF";

        if (options.radeonHd7000Fix) {
            // Force "Radeon 7?00" match to avoid matching "Radeon HD 7?00"
            stream << R"EOF(
         # GraphicsRulesMaker Tweak: Radeon HD 7000 series recognition
         stringMatch cardIdentity "Radeon 7?00*")EOF";
        }
        else {
            // Original value
            stream << R"EOF(
         stringMatch cardIdentity "*7?00*")EOF";
        }

        stream << R"EOF(
         stringMatch cardIdentity "*8?00*"
         stringMatch cardIdentity "*9000*"
         stringMatch cardIdentity "*9100*"
         stringMatch cardIdentity "*9200*"
         stringMatch cardIdentity "*9300*"
         stringMatch cardIdentity "*9400*"
         stringMatch cardIdentity "ATI, Radeon"
         stringMatch cardIdentity "ATI, Radeon VE"
         stringMatch cardIdentity "ATI, Radeon Mobility"
         stringMatch cardIdentity "ATI, Radeon FireGL"

         # results in special low-impact scrolling mode.
         property noPartialBackingStoreCopies true

         # try to reduce the number of dirty rects.
         property dirtyRectMergeFrames 8

         # use black and white cursors. The Radeons seem
         # to work with colour for a while, but in low
         # memory situations revert to flickering.
         option $Cursor $CursorBW
      end
   end

   partialRule "Kyro"
      # still doesn't work great -- has problems
      # minifying textures in zoom 1.
      stringMatch cardIdentity "*Kyro*"

      property useSecondStage false
      option $CloudsFog       $Off
      option $Waves           $Off
   end

   partialRule "Poor color cursor" -any
      stringMatch cardIdentity "ATI*128*"
      stringMatch cardIdentity "ATI*Rage*"
      stringMatch cardIdentity "ATI*Mach*"
      stringMatch cardIdentity "S3*"
      stringMatch cardIdentity "Trident*"
      stringMatch cardIdentity "3dfx*"        # The banshee reportedly works, but this is simplest.
      stringMatch cardIdentity "Intel*"       # 815E is BW only.  845+ is unknown but to be safe....

      option $Cursor          $CursorBW
   end

   partialRule "NoAddSigned" -any
      # this shows up as odd zone colours.
      stringMatch cardIdentity "ATI*128*"
      stringMatch cardIdentity "ATI*Rage*"

      property NoAddSigned true
   end

   partialRule "Not Slow GeForce"
      # We don't want the 'fast card' rules to apply to these GeForce cards, though possibly to other GeForce cards.
      stringNotMatch cardIdentity "*GeForce 256*"
      stringNotMatch cardIdentity "*GeForce2 *"

      partialRule "Fast card" -any)EOF";
        if (options.fastCard) {
            // Force any card to match as fast card
            stream << R"EOF(
         # GraphicsRulesMaker Tweak: Force card as fast graphics card
         stringMatch cardIdentity "*")EOF";
        }
        else {
        // Default detection
            stream << R"EOF(
         # We run well on these cards, so up the defaults.
         stringMatch cardIdentity "NVidia*Geforce*"
         stringMatch cardIdentity "NVidia*Quadro*"
         stringMatch cardIdentity "ATI*9700*"
         stringMatch cardIdentity "ATI*8500*"
         stringMatch cardIdentity "Matrox*Parhelia*"
)EOF";
        }
        stream << R"EOF(
         partialRule
            # Enable 32 bit graphics if user has decent computing power.
            # Paul P: My tests have shown that my GeForce uses less memory in 16 bit mode than 32 bit mode.
            atLeast memory          256
            atLeast cpuSpeed       1000
            option $ScreenDepth   $Depth_32
         end

         partialRule
            # Enable more cars and sims if user has decent computing power.
            atLeast memory          256
            atLeast cpuSpeed       1400
            option $CarsSims      $High
         end

         partialRule
            # Enable more visual effects if user has decent computing power.
            atLeast memory          256
            atLeast cpuSpeed       1600
            option $VisualEffects $High
         end

         partialRule
            # Enable more shadows if user has decent computing power.
            atLeast memory       512
            atLeast cpuSpeed    2000
            option $Shadows    $High
         end

         partialRule
            # Enable clouds and fog if user has decent computing power.
            atLeast memory          256
            atLeast cpuSpeed       1000
            option $CloudsFog     $On
         end

         property dirtyRectMergeFrames 2
      end
   end
)EOF";
    } // !allHighSettings
    stream << R"EOF(
end




# For figuring out what kind of card we are
sinclude "Video Cards.sgr"
)EOF";

}

SimCity4GameWriter::~SimCity4GameWriter()
{
}

#include "simcity4gamewriter.moc"
