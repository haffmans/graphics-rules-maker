cmake_minimum_required(VERSION 3.0.0)
project(Sims2GameWriter)

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
if (NOT GraphicRulesMaker_FULL_BUILD)
    find_package(GraphicRulesMaker REQUIRED)
endif (NOT GraphicRulesMaker_FULL_BUILD)

include_directories(${GraphicRulesMaker_INCLUDE_DIRS})

qt5_wrap_ui(GraphicRulesMaker_Sims2_UIS sims2settings.ui)

SET(GraphicRulesMaker_Sims2_SRCS
    sims2settings.cpp
    sims2gamewriter.cpp
    ${GraphicRulesMaker_Sims2_UIS}
)

add_library(Sims2GameWriter MODULE ${GraphicRulesMaker_Sims2_SRCS})
set_target_properties(Sims2GameWriter PROPERTIES PREFIX "")
target_link_libraries(Sims2GameWriter Qt5::Widgets GraphicRulesMaker)

install(TARGETS Sims2GameWriter
    RUNTIME DESTINATION ${GraphicRulesMaker_PLUGIN_PATH}
    LIBRARY DESTINATION ${GraphicRulesMaker_PLUGIN_PATH}
    COMPONENT sims2
)
