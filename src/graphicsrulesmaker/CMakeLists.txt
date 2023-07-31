cmake_minimum_required(VERSION 3.1.0)
project(GraphicsRulesMaker)

# Find other dependencies
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6Widgets 6.2 REQUIRED)
find_package(Qt6LinguistTools NO_CMAKE_FIND_ROOT_PATH)

if(WIN32)
    find_package(DirectX9 REQUIRED)
    include_directories(${DirectX9_INCLUDE_DIR})

    if(DirectX9_DXGI_LIBRARY)
        message(STATUS "DXGI Library found at " ${DirectX9_DXGI_LIBRARY})
        add_definitions(-DDXGI)
    endif(DirectX9_DXGI_LIBRARY)
endif(WIN32)

if (NOT DEFINED GraphicsRulesMaker_TRANSLATIONS_DIR)
    # This will put only this subproject's translations in a subdir within this project
    set(GraphicsRulesMaker_TRANSLATIONS_DIR "${PROJECT_SOURCE_DIR}/translations")
endif (NOT DEFINED GraphicsRulesMaker_TRANSLATIONS_DIR)

set(GraphicsRulesMaker_SRCS
    abstractsettingswidget.cpp
    gamewriterfactory.cpp
    graphicsruleswriter.cpp
    videocarddatabase.cpp
    devicemodel.cpp
    messagehandler.cpp
)

set(GraphicsRulesMaker_HDRS
    devicemodel.h
    videocarddatabase.h
    gamewriterfactory.h
    gamewriterinterface.h
    graphicsruleswriter.h
    abstractsettingswidget.h
    messagehandler.h
    ${PROJECT_BINARY_DIR}/graphicsrulesmaker_config.h
    ${CMAKE_CURRENT_BINARY_DIR}/pci.vendors.h
)

# Translations
if(${Qt6LinguistTools_FOUND})
    message(STATUS "Qt Linguist found - will set up translations")
    qt6_create_translation(GraphicsRulesMaker_QMS
        ${CMAKE_CURRENT_SOURCE_DIR}
        "${GraphicsRulesMaker_TRANSLATIONS_DIR}/GraphicsRulesMaker_nl.ts"
        OPTIONS -source-language "en_US"
    )
endif(${Qt6LinguistTools_FOUND})

# PCI Vendor ID database
add_custom_command(
    OUTPUT pci.vendors.h
    DEPENDS pci.ids pci.vendors.cmake
    MAIN_DEPENDENCY pci.ids
    COMMENT "Generating PCI vendors database"
    COMMAND ${CMAKE_COMMAND}
              "-DPCI_INPUT=${CMAKE_CURRENT_SOURCE_DIR}/pci.ids"
              "-DPCI_OUTPUT=${CMAKE_CURRENT_BINARY_DIR}/pci.vendors.h"
              "-DPCI_TEMPLATE=${CMAKE_CURRENT_SOURCE_DIR}/pci.vendors.h.in"
              "-P" "${CMAKE_CURRENT_SOURCE_DIR}/pci.vendors.cmake"
    VERBATIM
)
list(APPEND GraphicsRulesMaker_SRCS "pci.vendors.h")

# Determine installation path for CMake files
if(WIN32 AND NOT CYGWIN)
    set(INSTALL_CMAKE_DIR CMake)
else()
    set(INSTALL_CMAKE_DIR lib/cmake/GraphicsRulesMaker)
endif()

# Determine plugin installation path
# A bit of duplicate code (shared with the main project) to support split builds
if(NOT DEFINED GraphicsRulesMaker_PLUGIN_PATH)
    if(WIN32 AND NOT CYGWIN)
        set(PLUGIN_INSTALL_DIR bin/gamewriters/)
    else(WIN32 AND NOT CYGWIN)
        set(LIB_SUFFIX "" CACHE STRING "Define suffix of library directory name (eg. '64')")
        set(LIB_INSTALL_DIR lib${LIB_SUFFIX} )
        set(PLUGIN_INSTALL_DIR lib${LIB_SUFFIX}/graphicsrulesmaker/)
    endif(WIN32 AND NOT CYGWIN)
    set(GraphicsRulesMaker_PLUGIN_PATH ${CMAKE_INSTALL_PREFIX}/${PLUGIN_INSTALL_DIR} )
endif(NOT DEFINED GraphicsRulesMaker_PLUGIN_PATH)
# And for .qm files
if(NOT DEFINED GraphicsRulesMaker_TRANSLATIONS_PATH)
    if(WIN32 AND NOT CYGWIN)
        set(TRANSLATIONS_INSTALL_DIR share/)
    else(WIN32 AND NOT CYGWIN)
        set(TRANSLATIONS_INSTALL_DIR share/graphicsrulesmaker/)
    endif(WIN32 AND NOT CYGWIN)
    set(GraphicsRulesMaker_TRANSLATIONS_PATH ${CMAKE_INSTALL_PREFIX}/${TRANSLATIONS_INSTALL_DIR} )
endif(NOT DEFINED GraphicsRulesMaker_TRANSLATIONS_PATH)

# Version information
set(GraphicsRulesMaker_COPYRIGHT_YEAR 2023)
set(GraphicsRulesMaker_GIT_VERSION "" CACHE STRING "Version suffix, e.g. a git commit hash")
set(GraphicsRulesMaker_MAJOR_VERSION 2)
set(GraphicsRulesMaker_MINOR_VERSION 3)
set(GraphicsRulesMaker_PATCH_VERSION 0)

if (NOT GraphicsRulesMaker_GIT_VERSION STREQUAL "")
    set(GraphicsRulesMaker_VERSION_SUFFIX "-${GraphicsRulesMaker_GIT_VERSION}")
endif()

set(GraphicsRulesMaker_VERSION
  ${GraphicsRulesMaker_MAJOR_VERSION}.${GraphicsRulesMaker_MINOR_VERSION}.${GraphicsRulesMaker_PATCH_VERSION}${GraphicsRulesMaker_VERSION_SUFFIX})
message(STATUS "Building version: ${GraphicsRulesMaker_VERSION}")

# Configuration file
configure_file(graphicsrulesmaker_config.h.cmake
    ${PROJECT_BINARY_DIR}/graphicsrulesmaker_config.h @ONLY)

add_library(GraphicsRulesMaker SHARED ${GraphicsRulesMaker_SRCS} ${GraphicsRulesMaker_QMS})
target_compile_definitions(GraphicsRulesMaker PRIVATE GRAPHICSRULESMAKER_LIBRARY)
target_link_libraries(GraphicsRulesMaker Qt6::Widgets)
if(WIN32)
    # NOTE: ${DirectX9_DXGI_LIBRARY} is removed; we load it dynamically at runtime
    target_link_libraries(GraphicsRulesMaker ${DirectX9_LIBRARY} )
endif(WIN32)

if(WIN32)
    install(TARGETS GraphicsRulesMaker
        EXPORT GraphicsRulesMakerTargets
        RUNTIME DESTINATION bin
        COMPONENT application
    )
    # Different component for installer
    install(TARGETS GraphicsRulesMaker
        LIBRARY DESTINATION lib${LIB_SUFFIX}
        ARCHIVE DESTINATION lib${LIB_SUFFIX}
        COMPONENT dev
    )
else(WIN32)
    install(TARGETS GraphicsRulesMaker
        EXPORT GraphicsRulesMakerTargets
        LIBRARY DESTINATION lib${LIB_SUFFIX}
        ARCHIVE DESTINATION lib${LIB_SUFFIX}
        COMPONENT application
    )
endif(WIN32)

install(
    FILES ${GraphicsRulesMaker_HDRS}
    DESTINATION include/graphicsrulesmaker
    COMPONENT dev
)
install(
    FILES ${GraphicsRulesMaker_QMS}
    DESTINATION ${TRANSLATIONS_INSTALL_DIR}
    COMPONENT application
)

# Write out CMake files (for developers)
export(TARGETS GraphicsRulesMaker
    FILE "${PROJECT_BINARY_DIR}/GraphicsRulesMakerTargets.cmake")
# For installation (using absolute path on non-windows)
if(WIN32 AND NOT CYGWIN)
    set(CONF_INCLUDE_DIRS "\${GraphicsRulesMaker_CMAKE_DIR}/../include")
    set(GraphicsRulesMaker_EXPORT_PLUGIN_PATH "\${GraphicsRulesMaker_CMAKE_DIR}/../${PLUGIN_INSTALL_DIR}")
else(WIN32 AND NOT CYGWIN)
    set(CONF_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/include")
    set(GraphicsRulesMaker_EXPORT_PLUGIN_PATH "${GraphicsRulesMaker_PLUGIN_PATH}")
endif(WIN32 AND NOT CYGWIN)

configure_file(GraphicsRulesMakerConfig.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/GraphicsRulesMakerConfig.cmake" @ONLY)
# Version info
configure_file(GraphicsRulesMakerVersion.cmake.in
  "${PROJECT_BINARY_DIR}/GraphicsRulesMakerVersion.cmake" @ONLY)

install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/GraphicsRulesMakerConfig.cmake"
  "${PROJECT_BINARY_DIR}/GraphicsRulesMakerVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}"
  COMPONENT dev
)

install(EXPORT GraphicsRulesMakerTargets
    DESTINATION "${INSTALL_CMAKE_DIR}"
    COMPONENT dev
)
