cmake_minimum_required(VERSION 3.0.0)
project(GraphicRulesMakerBundle)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_BINARY_DIR}")

# Tell subprojects we're doing a full build (including library)
set(GraphicRulesMaker_FULL_BUILD 1)

# Required to get "graphicrulesmaker/*" includes working for the UI/plugins,
# as with the split builds
include_directories(
  "${PROJECT_SOURCE_DIR}/src"
  "${PROJECT_BINARY_DIR}/src"
)

# Determine plugin installation directory
if(WIN32)
    set(PLUGIN_INSTALL_DIR bin/gamewriters/)
else(WIN32)
    set(LIB_SUFFIX "" CACHE STRING "Define suffix of library directory name (eg. '64')")
    set(LIB_INSTALL_DIR lib${LIB_SUFFIX} )
    set(PLUGIN_INSTALL_DIR lib${LIB_SUFFIX}/graphicrulesmaker/)
endif(WIN32)
set(GraphicRulesMaker_PLUGIN_PATH ${CMAKE_INSTALL_PREFIX}/${PLUGIN_INSTALL_DIR} )

# Add modules
add_subdirectory(src/graphicrulesmaker)
add_subdirectory(src/graphicrulesmakerui)
add_subdirectory(src/plugins)

# Copy required Qt5 DLLs for Windows
if(WIN32)
    get_target_property(Qt5Core_Location Qt5::Core LOCATION)
    get_target_property(Qt5Gui_Location Qt5::Gui LOCATION)
    get_target_property(Qt5Widgets_Location Qt5::Widgets LOCATION)

    get_target_property(Qt5Gui_WindowsPlugin_Location Qt5::QWindowsIntegrationPlugin LOCATION)

    # Assume MingW/ICU dlls are with QtCore
    get_filename_component(Qt5Core_BinDir "${Qt5Core_Location}" DIRECTORY)

    if(MINGW)
        # Yep, this could be cleaner, using find_library and whatnot. Can't be bothered for now.
        SET(Qt5Core_Dependencies
                ${Qt5Core_BinDir}/icudt52.dll
                ${Qt5Core_BinDir}/icuin52.dll
                ${Qt5Core_BinDir}/icuuc52.dll
                ${Qt5Core_BinDir}/libgcc_s_dw2-1.dll
                ${Qt5Core_BinDir}/libstdc++-6.dll
                ${Qt5Core_BinDir}/libwinpthread-1.dll
        )
    endif(MINGW)

    install(FILES ${Qt5Core_Location}
                  ${Qt5Gui_Location}
                  ${Qt5Widgets_Location}
                  ${Qt5Core_Dependencies}
            DESTINATION bin
            COMPONENT application
    )

    install(FILES ${Qt5Gui_WindowsPlugin_Location}
            DESTINATION bin/platforms
            COMPONENT application
    )

    install(FILES "${PROJECT_SOURCE_DIR}/simsnetwork.ico"
                "${PROJECT_SOURCE_DIR}/LICENSE.txt"
                "${PROJECT_SOURCE_DIR}/Readme.txt"
            DESTINATION .
            COMPONENT application
    )
endif(WIN32)

include(InstallRequiredSystemLibraries)
include(CPack)
