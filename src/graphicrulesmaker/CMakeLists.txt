cmake_minimum_required(VERSION 3.0.0)
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

# Find other dependencies
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED)

if(WIN32)
    find_package(DirectX REQUIRED)
    include_directories(${DirectX_INCLUDE_DIR})
endif(WIN32)

set(GraphicRulesMaker_SRCS
    videocarddatabase.cpp
    devicemodel.cpp
)

set(GraphicRulesMaker_HDRS
    "devicemodel.h"
    "videocarddatabase.h"
    "gamewriterinterface.h"
    "${PROJECT_BINARY_DIR}/graphicrulesmaker_config.h"
)

# Determine installation path for CMake files
if(WIN32 AND NOT CYGWIN)
    set(INSTALL_CMAKE_DIR CMake)
else()
    set(INSTALL_CMAKE_DIR lib/cmake/GraphicRulesMaker)
endif()

# Determine plugin installation path
# A bit of duplicate code (shared with the main project) to support split builds
if(NOT GraphicRulesMaker_PLUGIN_PATH)
    if(WIN32)
        set(PLUGIN_INSTALL_DIR bin/gamewriters/)
    else(WIN32)
        set(LIB_SUFFIX "" CACHE STRING "Define suffix of library directory name (eg. '64')")
        set(LIB_INSTALL_DIR lib${LIB_SUFFIX} )
        set(PLUGIN_INSTALL_DIR lib${LIB_SUFFIX}/graphicrulesmaker/)
    endif(WIN32)
    set(GraphicRulesMaker_PLUGIN_PATH ${CMAKE_INSTALL_PREFIX}/${PLUGIN_INSTALL_DIR} )
endif(NOT GraphicRulesMaker_PLUGIN_PATH)

# Version information
set(GraphicRulesMaker_MAJOR_VERSION 1)
set(GraphicRulesMaker_MINOR_VERSION 0)
set(GraphicRulesMaker_PATCH_VERSION 0)
set(GraphicRulesMaker_VERSION
  ${GraphicRulesMaker_MAJOR_VERSION}.${GraphicRulesMaker_MINOR_VERSION}.${GraphicRulesMaker_PATCH_VERSION})

# Configuration file
configure_file(graphicrulesmaker_config.h.cmake
    ${PROJECT_BINARY_DIR}/graphicrulesmaker_config.h @ONLY)

add_library(GraphicRulesMaker SHARED ${GraphicRulesMaker_SRCS})
target_link_libraries(GraphicRulesMaker Qt5::Widgets)
if(WIN32)
    target_link_libraries(GraphicRulesMaker ${DirectX_LIBRARIES})
endif(WIN32)

if(WIN32)
    install(TARGETS GraphicRulesMaker
        EXPORT GraphicRulesMakerTargets
        RUNTIME DESTINATION bin
        COMPONENT application
    )
    # Different component for installer
    install(TARGETS GraphicRulesMaker
        LIBRARY DESTINATION lib${LIB_SUFFIX}
        ARCHIVE DESTINATION lib${LIB_SUFFIX}
        COMPONENT dev
    )
else(WIN32)
    install(TARGETS GraphicRulesMaker
        EXPORT GraphicRulesMakerTargets
        LIBRARY DESTINATION lib${LIB_SUFFIX}
        ARCHIVE DESTINATION lib${LIB_SUFFIX}
        COMPONENT application
    )
endif(WIN32)

install(
    FILES ${GraphicRulesMaker_HDRS}
    DESTINATION include/graphicrulesmaker
    COMPONENT dev
)

# Write out CMake files (for developers)
export(TARGETS GraphicRulesMaker
    FILE "${PROJECT_BINARY_DIR}/GraphicRulesMakerTargets.cmake")
# For installation (using absolute path)
set(CONF_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/include")
configure_file(GraphicRulesMakerConfig.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/GraphicRulesMakerConfig.cmake" @ONLY)
# Version info
configure_file(GraphicRulesMakerVersion.cmake.in
  "${PROJECT_BINARY_DIR}/GraphicRulesMakerVersion.cmake" @ONLY)

install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/GraphicRulesMakerConfig.cmake"
  "${PROJECT_BINARY_DIR}/GraphicRulesMakerVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}"
  COMPONENT dev
)

install(EXPORT GraphicRulesMakerTargets
    DESTINATION "${INSTALL_CMAKE_DIR}"
    COMPONENT dev
)
