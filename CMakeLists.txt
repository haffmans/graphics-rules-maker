cmake_minimum_required(VERSION 2.8.11)
project(GraphicRulesMaker)

# We need C++11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(Qt5Widgets)

if(WIN32)
    find_package(DirectX REQUIRED)
    include_directories(${DirectX_INCLUDE_DIR})
endif(WIN32)

if(WIN32)
    set(PLUGIN_INSTALL_DIR bin/gamewriters/)
else(WIN32)
    set(LIB_SUFFIX "" CACHE STRING "Define suffix of library directory name (eg. '64')")
    set(LIB_INSTALL_DIR lib${LIB_SUFFIX} )
    set(PLUGIN_INSTALL_DIR lib${LIB_SUFFIX}/graphicrulesmaker/)
endif(WIN32)
set(GraphicRulesMaker_PLUGIN_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/${PLUGIN_INSTALL_DIR} )

configure_file(graphicrulesmaker_config.h.cmake  ${PROJECT_BINARY_DIR}/graphicrulesmaker_config.h)

qt5_wrap_ui(GraphicRulesMaker_UIS
    mainwindow.ui
    manualsaveconfirmationbox.ui
    aboutdialog.ui
)

# libGraphicRulesMaker
set(GraphicRulesMaker_SRCS
    videocarddatabase.cpp
    devicemodel.cpp
)

add_library(GraphicRulesMaker SHARED ${GraphicRulesMaker_SRCS})
target_link_libraries(GraphicRulesMaker Qt5::Widgets)

if(WIN32)
    target_link_libraries(GraphicRulesMaker ${DirectX_LIBRARIES})
endif(WIN32)

# GraphicRulesMaker UI
set(GraphicRulesMakerUi_SRCS
    main.cpp
    mainwindow.cpp
    manualsaveconfirmationbox.cpp
    aboutdialog.cpp
    gamewriterfactory.cpp
    ${GraphicRulesMaker_UIS}
)

# Set some Win32 Specific Settings
IF(WIN32)
    SET(GUI_TYPE WIN32)
ENDIF(WIN32)
# Set some Apple MacOS Specific settings
IF (APPLE)
    SET(GUI_TYPE MACOSX_BUNDLE)
ENDIF (APPLE)

add_executable(GraphicRulesMakerUi ${GUI_TYPE} ${GraphicRulesMakerUi_SRCS})
target_link_libraries(GraphicRulesMakerUi GraphicRulesMaker Qt5::Widgets)

if(WIN32)
    target_link_libraries(GraphicRulesMakerUi ${DirectX_LIBRARIES})
endif(WIN32)

# Common install
install(TARGETS GraphicRulesMaker GraphicRulesMakerUi
    RUNTIME DESTINATION bin
    COMPONENT application
)
install(TARGETS GraphicRulesMaker
    LIBRARY DESTINATION lib${LIB_SUFFIX}
    ARCHIVE DESTINATION lib${LIB_SUFFIX}
    COMPONENT dev
)
install(FILES
        "${PROJECT_SOURCE_DIR}/devicemodel.h"
        "${PROJECT_SOURCE_DIR}/videocarddatabase.h"
        "${PROJECT_SOURCE_DIR}/gamewriterinterface.h"
    DESTINATION include
    COMPONENT dev
)

add_subdirectory(sims2)

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

if(MINGW AND WIN32)


    # this is used when searching for libraries e.g. using the FIND_LIBRARY() command.
    MESSAGE( STATUS "CMAKE_LIBRARY_PATH: " ${CMAKE_C_COMPILER} )

endif(MINGW AND WIN32)


# CPack stuff
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Graphic Rules Maker")
set(CPACK_PACKAGE_VENDOR "SimsNetwork.com")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Graphic Rules Maker")

# Components: application, dev, sims2
set(CPACK_COMPONENTS_ALL application dev sims2)

set(CPACK_COMPONENT_APPLICATION_DISPLAY_NAME "Graphic Rules Maker Application")
set(CPACK_COMPONENT_APPLICATION_DESCRIPTION "The application and all required files.")
set(CPACK_COMPONENT_APPLICATION_REQUIRED 1)
set(CPACK_COMPONENT_APPLICATION_INSTALL_TYPES Default Dev)
set(CPACK_COMPONENT_APPLICATION_GROUP App)

set(CPACK_COMPONENT_DEV_DISPLAY_NAME "Development files")
set(CPACK_COMPONENT_DEV_DESCRIPTION "Files and headers required to develop new writer plugins.")
set(CPACK_COMPONENT_DEV_INSTALL_TYPES Dev)
set(CPACK_COMPONENT_DEV_DISABLED 1)
set(CPACK_COMPONENT_DEV_GROUP App)

set(CPACK_COMPONENT_SIMS2_DISPLAY_NAME "Sims 2")
set(CPACK_COMPONENT_SIMS2_DESCRIPTION "Sims 2 plugin")
set(CPACK_COMPONENT_SIMS2_REQUIRED 1)
set(CPACK_COMPONENT_SIMS2_INSTALL_TYPES Default Dev)
set(CPACK_COMPONENT_SIMS2_GROUP Plugins)
set(CPACK_COMPONENT_SIMS2_DEPENDS application)

set(CPACK_COMPONENT_GROUP_APP_DISPLAY_NAME "Graphic Rules Maker")
set(CPACK_COMPONENT_GROUP_APP_DESCRIPTION "The application itself.")
set(CPACK_COMPONENT_GROUP_APP_EXPANDED 0)

set(CPACK_COMPONENT_GROUP_PLUGINS_DISPLAY_NAME "Plugins")
set(CPACK_COMPONENT_GROUP_PLUGINS_DESCRIPTION "Game plugins")
set(CPACK_COMPONENT_GROUP_PLUGINS_EXPANDED 1)

set(CPACK_INSTALL_TYPE_DEFAULT_DISPLAY_NAME "Default Installation")
set(CPACK_INSTALL_TYPE_DEV_DISPLAY_NAME "Developers")

if(WIN32 AND NOT UNIX)
    set(CPACK_GENERATOR "NSIS")

    # There is a bug in NSI that does not handle full unix paths properly. Make
    # sure there is at least one set of four (4) backlasshes.
    # set(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}\\\\InstallIcon.bmp")
    # set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\GraphicRulesMakerUI.exe")
    set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY}")
    set(CPACK_NSIS_HELP_LINK "http://www.simsnetwork.com")
    set(CPACK_NSIS_URL_INFO_ABOUT "http://www.simsnetwork.com")
    set(CPACK_NSIS_CONTACT "wouter@simply-life.net")
    set(CPACK_NSIS_MODIFY_PATH OFF)
    set(CPACK_NSIS_CREATE_ICONS_EXTRA "!insertmacro CreateInternetShortcut '\$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\SimsNetwork.url' 'http://www.simsnetwork.com' '$INSTDIR\\\\simsnetwork.ico' 0")
    set(CPACK_NSIS_DELETE_ICONS_EXTRA "Delete '\$SMPROGRAMS\\\\$MUI_TEMP\\\\SimsNetwork.url'")
    set(CPACK_NSIS_MUI_FINISHPAGE_RUN "GraphicRulesMakerUI.exe")
else(WIN32 AND NOT UNIX)
    set(CPACK_STRIP_FILES "bin/GraphicRulesMakerUI")
    set(CPACK_SOURCE_STRIP_FILES "")
endif(WIN32 AND NOT UNIX)
set(CPACK_PACKAGE_EXECUTABLES "GraphicRulesMakerUI" "Graphic Rules Maker")
include(CPack)
