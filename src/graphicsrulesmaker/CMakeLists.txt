cmake_minimum_required(VERSION 3.0.0)
project(GraphicsRulesMaker)

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

# Find other dependencies
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED)

if(WIN32)
    find_package(DirectX REQUIRED)
    include_directories(${DirectX_INCLUDE_DIR})
endif(WIN32)

set(GraphicsRulesMaker_SRCS
    videocarddatabase.cpp
    devicemodel.cpp
)

set(GraphicsRulesMaker_HDRS
    "devicemodel.h"
    "videocarddatabase.h"
    "gamewriterinterface.h"
    "${PROJECT_BINARY_DIR}/graphicsrulesmaker_config.h"
)

# Determine installation path for CMake files
if(WIN32 AND NOT CYGWIN)
    set(INSTALL_CMAKE_DIR CMake)
else()
    set(INSTALL_CMAKE_DIR lib/cmake/GraphicsRulesMaker)
endif()

# Determine plugin installation path
# A bit of duplicate code (shared with the main project) to support split builds
if(NOT GraphicsRulesMaker_PLUGIN_PATH)
    if(WIN32)
        set(PLUGIN_INSTALL_DIR bin/gamewriters/)
    else(WIN32)
        set(LIB_SUFFIX "" CACHE STRING "Define suffix of library directory name (eg. '64')")
        set(LIB_INSTALL_DIR lib${LIB_SUFFIX} )
        set(PLUGIN_INSTALL_DIR lib${LIB_SUFFIX}/graphicsrulesmaker/)
    endif(WIN32)
    set(GraphicsRulesMaker_PLUGIN_PATH ${CMAKE_INSTALL_PREFIX}/${PLUGIN_INSTALL_DIR} )
endif(NOT GraphicsRulesMaker_PLUGIN_PATH)

# Version information
set(GraphicsRulesMaker_MAJOR_VERSION 1)
set(GraphicsRulesMaker_MINOR_VERSION 0)
set(GraphicsRulesMaker_PATCH_VERSION 0)
set(GraphicsRulesMaker_VERSION
  ${GraphicsRulesMaker_MAJOR_VERSION}.${GraphicsRulesMaker_MINOR_VERSION}.${GraphicsRulesMaker_PATCH_VERSION})

# Configuration file
configure_file(graphicsrulesmaker_config.h.cmake
    ${PROJECT_BINARY_DIR}/graphicsrulesmaker_config.h @ONLY)

add_library(GraphicsRulesMaker SHARED ${GraphicsRulesMaker_SRCS})
target_link_libraries(GraphicsRulesMaker Qt5::Widgets)
if(WIN32)
    target_link_libraries(GraphicsRulesMaker ${DirectX_LIBRARIES})
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
