cmake_minimum_required(VERSION 3.0.0)
project(GraphicsRulesMakerBundle)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_BINARY_DIR}")

# Tell subprojects we're doing a full build (including library)
set(GraphicsRulesMaker_FULL_BUILD 1)

# Required to get "graphicsrulesmaker/*" includes working for the UI/plugins,
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
    set(PLUGIN_INSTALL_DIR lib${LIB_SUFFIX}/graphicsrulesmaker/)
endif(WIN32)
set(GraphicsRulesMaker_PLUGIN_PATH ${CMAKE_INSTALL_PREFIX}/${PLUGIN_INSTALL_DIR} )

# Put all translations together for more convenience
set(GraphicsRulesMaker_TRANSLATIONS_DIR "${PROJECT_SOURCE_DIR}/translations")

# And install them to the same path too
if(WIN32 AND NOT CYGWIN)
    set(TRANSLATIONS_INSTALL_DIR share/)
else(WIN32 AND NOT CYGWIN)
    set(TRANSLATIONS_INSTALL_DIR share/graphicsrulesmaker/)
endif(WIN32 AND NOT CYGWIN)


# Add modules
add_subdirectory(src/graphicsrulesmaker)
add_subdirectory(src/graphicsrulesmakerui)
add_subdirectory(src/plugins)

if(WIN32)
    find_package(Qt5Widgets REQUIRED)
    find_package(ImageMagick REQUIRED COMPONENTS convert)

    # Generate NSIS Header
    install(CODE "message(STATUS \"Creating NSIS Header: \\\"${PROJECT_BINARY_DIR}/header.bmp\\\" \")
              execute_process(COMMAND \"${IMAGEMAGICK_CONVERT_EXECUTABLE}\" \"${PROJECT_SOURCE_DIR}/src/graphicsrulesmakerui/icon.png\" -resize x48 -gravity EAST -extent 150x57 \"BMP3:${PROJECT_BINARY_DIR}/header.bmp\" )"
        COMPONENT application)

    # Generate SimsNetwork.com icon
    add_custom_command(OUTPUT "simsnetwork-64.png" "simsnetwork-48.png" "simsnetwork-32.png"
        COMMAND ${IMAGEMAGICK_CONVERT_EXECUTABLE} "${PROJECT_SOURCE_DIR}/simsnetwork-128.png" -resize 64x64 "${PROJECT_BINARY_DIR}/simsnetwork-64.png"
        COMMAND ${IMAGEMAGICK_CONVERT_EXECUTABLE} "${PROJECT_SOURCE_DIR}/simsnetwork-128.png" -resize 48x48 "${PROJECT_BINARY_DIR}/simsnetwork-48.png"
        COMMAND ${IMAGEMAGICK_CONVERT_EXECUTABLE} "${PROJECT_SOURCE_DIR}/simsnetwork-128.png" -resize 32x32 "${PROJECT_BINARY_DIR}/simsnetwork-32.png"
        COMMAND ${IMAGEMAGICK_CONVERT_EXECUTABLE} "${PROJECT_SOURCE_DIR}/simsnetwork-128.png" -resize 16x16 "${PROJECT_BINARY_DIR}/simsnetwork-16.png"
        COMMENT "Resizing SimsNetwork icon"
        MAIN_DEPENDENCY "${PROJECT_SOURCE_DIR}/simsnetwork-128.png"
    )

    add_custom_command(OUTPUT "simsnetwork.ico"
        COMMAND ${IMAGEMAGICK_CONVERT_EXECUTABLE}
            "${PROJECT_SOURCE_DIR}/simsnetwork-128.png"
            "simsnetwork-64.png"
            "simsnetwork-48.png"
            "simsnetwork-32.png"
            "${PROJECT_SOURCE_DIR}/simsnetwork-16.ico"
            "simsnetwork.ico"
        COMMENT "Creating SimsNetwork final icon"
        MAIN_DEPENDENCY "${PROJECT_SOURCE_DIR}/simsnetwork-128.png"
        DEPENDS "simsnetwork-64.png" "simsnetwork-48.png" "simsnetwork-32.png" "${PROJECT_SOURCE_DIR}/simsnetwork-16.ico"
    )
    add_custom_target(GraphicsRulesMakerIcons ALL
        DEPENDS "simsnetwork.ico"
        SOURCES "simsnetwork-16.ico" "simsnetwork-128.png")

    install(FILES "${PROJECT_BINARY_DIR}/simsnetwork.ico"
                "${PROJECT_SOURCE_DIR}/LICENSE.txt"
                "${PROJECT_SOURCE_DIR}/Readme.txt"
            DESTINATION .
            COMPONENT application
    )
endif(WIN32)

include(InstallRequiredSystemLibraries)
include("${PROJECT_SOURCE_DIR}/CPackConfig.cmake")
include(CPack)

