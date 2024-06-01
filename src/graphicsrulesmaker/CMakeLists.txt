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

option(WITH_QT5 "Build against Qt5 (5.15) instead of Qt6" OFF)
set(GraphicsRulesMaker_WITH_QT5 ${WITH_QT5})

if (NOT WITH_QT5)
    find_package(Qt6Widgets 6.2 REQUIRED)
else()
    find_package(Qt5Widgets 5.15 REQUIRED)
endif()

if(WIN32)
    find_package(DirectX9 REQUIRED)
    include_directories(${DirectX9_INCLUDE_DIR})

    if(DirectX9_DXGI_LIBRARY)
        message(STATUS "DXGI Library found at " ${DirectX9_DXGI_LIBRARY})
        add_definitions(-DDXGI)
    endif(DirectX9_DXGI_LIBRARY)
endif(WIN32)

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

# Determine installation paths (CMake files, plugins and translation .qm files)
include(GNUInstallDirs)
if(WIN32 AND NOT CYGWIN)
    set(GraphicsRulesMaker_INSTALL_CMAKE_DIR CMake)
    set(GraphicsRulesMaker_INSTALL_LIB_DIR ${CMAKE_INSTALL_LIBDIR})
    set(GraphicsRulesMaker_INSTALL_PLUGIN_DIR ${CMAKE_INSTALL_BINDIR}/gamewriters/)
    set(GraphicsRulesMaker_INSTALL_TRANSLATIONS_DIR ${CMAKE_INSTALL_DATADIR})
    set(GraphicsRulesMaker_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR})
else()
    set(GraphicsRulesMaker_INSTALL_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/GraphicsRulesMaker)
    set(GraphicsRulesMaker_INSTALL_LIB_DIR ${CMAKE_INSTALL_LIBDIR})
    set(GraphicsRulesMaker_INSTALL_PLUGIN_DIR ${CMAKE_INSTALL_LIBDIR}/graphicsrulesmaker)
    set(GraphicsRulesMaker_INSTALL_TRANSLATIONS_DIR ${CMAKE_INSTALL_DATADIR}/graphicsrulesmaker/)
    set(GraphicsRulesMaker_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR})
endif()

if (NOT PROJECT_IS_TOP_LEVEL)
    # For the all-in-one build, so find_package(GraphicsRulesMaker) works as expected
    set(GraphicsRulesMaker_BUILD_CONFIG_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/${GraphicsRulesMaker_INSTALL_CMAKE_DIR}" PARENT_SCOPE)
endif()
set(GraphicsRulesMaker_FULL_PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/${GraphicsRulesMaker_INSTALL_PLUGIN_DIR} )
set(GraphicsRulesMaker_FULL_TRANSLATIONS_DIR ${CMAKE_INSTALL_PREFIX}/${GraphicsRulesMaker_INSTALL_TRANSLATIONS_DIR} )
include(GraphicsRulesMakerMacros.cmake)

# Configuration file
configure_file(graphicsrulesmaker_config.h.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/graphicsrulesmaker_config.h @ONLY)

add_library(GraphicsRulesMaker SHARED ${GraphicsRulesMaker_SRCS})
target_compile_definitions(GraphicsRulesMaker PRIVATE GRAPHICSRULESMAKER_LIBRARY)
if(NOT GraphicsRulesMaker_WITH_QT5)
    target_link_libraries(GraphicsRulesMaker PUBLIC Qt6::Widgets)
else()
    target_link_libraries(GraphicsRulesMaker PUBLIC Qt5::Widgets)
endif()
target_include_directories(GraphicsRulesMaker INTERFACE
    $<INSTALL_INTERFACE:include>
    $<INSTALL_INTERFACE:include/graphicsrulesmaker>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>
)
graphicsrulesmaker_translations(GraphicsRulesMaker LANGUAGES nl)

if(WIN32)
    # NOTE: ${DirectX9_DXGI_LIBRARY} is removed; we load it dynamically at runtime
    target_link_libraries(GraphicsRulesMaker PRIVATE ${DirectX9_LIBRARY} )
endif(WIN32)

# Package installation
include(CMakePackageConfigHelpers)
if(WIN32)
    set(GraphicsRulesMaker_INSTALL_LIB_COMPONENT application)
else()
    set(GraphicsRulesMaker_INSTALL_LIB_COMPONENT dev)
endif()

install(TARGETS GraphicsRulesMaker
    EXPORT GraphicsRulesMakerTargets
    RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT application
    LIBRARY
        DESTINATION ${GraphicsRulesMaker_INSTALL_LIB_DIR}
        COMPONENT ${GraphicsRulesMaker_INSTALL_LIB_COMPONENT}
    ARCHIVE
        DESTINATION ${GraphicsRulesMaker_INSTALL_LIB_DIR}
        COMPONENT ${GraphicsRulesMaker_INSTALL_LIB_COMPONENT}
)

install(
    FILES ${GraphicsRulesMaker_HDRS}
    DESTINATION ${GraphicsRulesMaker_INSTALL_INCLUDE_DIR}/graphicsrulesmaker
    COMPONENT dev
)
install(
    FILES ${GraphicsRulesMaker_TRANSLATIONS}
    DESTINATION ${GraphicsRulesMaker_INSTALL_TRANSLATIONS_DIR}
    COMPONENT application
)

# Write out CMake files (for developers)
export(TARGETS GraphicsRulesMaker
    FILE "${CMAKE_CURRENT_BINARY_DIR}/${GraphicsRulesMaker_INSTALL_CMAKE_DIR}/GraphicsRulesMakerTargets.cmake")

configure_package_config_file(GraphicsRulesMakerConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/${GraphicsRulesMaker_INSTALL_CMAKE_DIR}/GraphicsRulesMakerConfig.cmake"
  INSTALL_DESTINATION "${GraphicsRulesMaker_INSTALL_CMAKE_DIR}"
  PATH_VARS
    GraphicsRulesMaker_INSTALL_INCLUDE_DIR
    GraphicsRulesMaker_INSTALL_PLUGIN_DIR
    GraphicsRulesMaker_INSTALL_TRANSLATIONS_DIR
)
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/${GraphicsRulesMaker_INSTALL_CMAKE_DIR}/GraphicsRulesMakerVersion.cmake"
    COMPATIBILITY ExactVersion
)
configure_file(GraphicsRulesMakerMacros.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${GraphicsRulesMaker_INSTALL_CMAKE_DIR}/GraphicsRulesMakerMacros.cmake COPYONLY)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${GraphicsRulesMaker_INSTALL_CMAKE_DIR}/GraphicsRulesMakerConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${GraphicsRulesMaker_INSTALL_CMAKE_DIR}/GraphicsRulesMakerVersion.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${GraphicsRulesMaker_INSTALL_CMAKE_DIR}/GraphicsRulesMakerMacros.cmake"
  DESTINATION "${GraphicsRulesMaker_INSTALL_CMAKE_DIR}"
  COMPONENT dev
)

install(EXPORT GraphicsRulesMakerTargets
    DESTINATION "${GraphicsRulesMaker_INSTALL_CMAKE_DIR}"
    COMPONENT dev
)
