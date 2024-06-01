cmake_minimum_required(VERSION 3.24)
project(GraphicsRulesMaker
    VERSION 2.3.0.0
    DESCRIPTION "Graphics Rules Maker"
    HOMEPAGE_URL "http://www.simsnetwork.com"
    LANGUAGES CXX
)

# Version information
set(GraphicsRulesMaker_COPYRIGHT_YEAR 2024)
set(GraphicsRulesMaker_GIT_VERSION "" CACHE STRING "Version suffix, e.g. a git commit hash")

if (NOT GraphicsRulesMaker_GIT_VERSION STREQUAL "")
    set(GraphicsRulesMaker_VERSION
        ${GraphicsRulesMaker_VERSION}${GraphicsRulesMaker_VERSION_SUFFIX})
endif()

message(STATUS "Building version: ${GraphicsRulesMaker_VERSION}")

# Language/dependency setup
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

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
    ${CMAKE_CURRENT_BINARY_DIR}/graphicsrulesmaker_config.h
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
# For the all-in-one build, so find_package(GraphicsRulesMaker) works as expected
set(GraphicsRulesMaker_BUILD_CONFIG_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/${INSTALL_CMAKE_DIR}" PARENT_SCOPE)

# Determine plugin installation path
if(WIN32 AND NOT CYGWIN)
    set(GraphicsRulesMaker_PLUGIN_INSTALL_DIR bin/gamewriters/)
else(WIN32 AND NOT CYGWIN)
    set(LIB_SUFFIX "" CACHE STRING "Define suffix of library directory name (eg. '64')")
    set(LIB_INSTALL_DIR lib${LIB_SUFFIX} )
    set(GraphicsRulesMaker_PLUGIN_INSTALL_DIR lib${LIB_SUFFIX}/graphicsrulesmaker/)
endif(WIN32 AND NOT CYGWIN)
set(GraphicsRulesMaker_PLUGIN_PATH ${CMAKE_INSTALL_PREFIX}/${GraphicsRulesMaker_PLUGIN_INSTALL_DIR} )

# And for .qm files
if(WIN32 AND NOT CYGWIN)
    set(TRANSLATIONS_INSTALL_DIR share/)
else(WIN32 AND NOT CYGWIN)
    set(TRANSLATIONS_INSTALL_DIR share/graphicsrulesmaker/)
endif(WIN32 AND NOT CYGWIN)
set(GraphicsRulesMaker_TRANSLATIONS_PATH ${CMAKE_INSTALL_PREFIX}/${TRANSLATIONS_INSTALL_DIR} )

# Configuration file
configure_file(graphicsrulesmaker_config.h.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/graphicsrulesmaker_config.h @ONLY)

add_library(GraphicsRulesMaker SHARED ${GraphicsRulesMaker_SRCS} ${GraphicsRulesMaker_QMS})
target_compile_definitions(GraphicsRulesMaker PRIVATE GRAPHICSRULESMAKER_LIBRARY)
target_link_libraries(GraphicsRulesMaker Qt6::Widgets)
target_include_directories(GraphicsRulesMaker INTERFACE
    $<INSTALL_INTERFACE:include>
    $<INSTALL_INTERFACE:include/graphicsrulesmaker>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>
)
if(WIN32)
    # NOTE: ${DirectX9_DXGI_LIBRARY} is removed; we load it dynamically at runtime
    target_link_libraries(GraphicsRulesMaker ${DirectX9_LIBRARY} )
endif(WIN32)

# Package installation
include(CMakePackageConfigHelpers)
if(WIN32)
    set(INSTALL_LIB_COMPONENT application)
else()
    set(INSTALL_LIB_COMPONENT dev)
endif()

install(TARGETS GraphicsRulesMaker
    EXPORT GraphicsRulesMakerTargets
    RUNTIME
        DESTINATION bin
        COMPONENT application
    LIBRARY
        DESTINATION lib${LIB_SUFFIX}
        COMPONENT ${INSTALL_LIB_COMPONENT}
    ARCHIVE
        DESTINATION lib${LIB_SUFFIX}
        COMPONENT ${INSTALL_LIB_COMPONENT}
)

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
    FILE "${CMAKE_CURRENT_BINARY_DIR}/${INSTALL_CMAKE_DIR}/GraphicsRulesMakerTargets.cmake")

set(GraphicsRulesMaker_INCLUDE_INSTALL_PATH "include")

configure_package_config_file(GraphicsRulesMakerConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/${INSTALL_CMAKE_DIR}/GraphicsRulesMakerConfig.cmake"
  INSTALL_DESTINATION "${INSTALL_CMAKE_DIR}"
  PATH_VARS
    GraphicsRulesMaker_INCLUDE_INSTALL_PATH
    GraphicsRulesMaker_PLUGIN_INSTALL_DIR
    TRANSLATIONS_INSTALL_DIR
)
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/${INSTALL_CMAKE_DIR}/GraphicsRulesMakerVersion.cmake"
    COMPATIBILITY ExactVersion
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${INSTALL_CMAKE_DIR}/GraphicsRulesMakerConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${INSTALL_CMAKE_DIR}/GraphicsRulesMakerVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}"
  COMPONENT dev
)

install(EXPORT GraphicsRulesMakerTargets
    DESTINATION "${INSTALL_CMAKE_DIR}"
    COMPONENT dev
)
