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
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

find_package(Qt5Widgets)

if(WIN32)
    find_package(DirectX REQUIRED)
    include_directories(${DirectX_INCLUDE_DIR})
endif(WIN32)

set(LIB_SUFFIX "" CACHE STRING "Define suffix of library directory name (eg. '64')")
set(LIB_INSTALL_DIR lib${LIB_SUFFIX} )
set(PLUGIN_INSTALL_DIR lib${LIB_SUFFIX}/graphicrulesmaker/)

set(GraphicRulesMaker_PLUGIN_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/${PLUGIN_INSTALL_DIR} )

configure_file(graphicrulesmaker_config.h.cmake  ${PROJECT_BINARY_DIR}/graphicrulesmaker_config.h)

qt5_wrap_ui(GraphicRulesMaker_UIS mainwindow.ui manualsaveconfirmationbox.ui)

# libGraphicRulesMaker
set(GraphicRulesMaker_SRCS
    videocarddatabase.cpp
    devicemodel.cpp
    gamewriterfactory.cpp
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
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}
)

add_subdirectory(sims2)
