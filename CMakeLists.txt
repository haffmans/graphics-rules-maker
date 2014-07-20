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
find_package(Qt5Widgets)

qt5_wrap_ui(GraphicRulesMaker_UIS mainwindow.ui)

SET(GraphicRulesMaker_SRCS
    main.cpp
    mainwindow.cpp
    videocarddatabase.cpp
    devicemodel.cpp
    ${GraphicRulesMaker_UIS}
)

add_executable(GraphicRulesMaker ${GraphicRulesMaker_SRCS})
target_link_libraries(GraphicRulesMaker Qt5::Widgets)
