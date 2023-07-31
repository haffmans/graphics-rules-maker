cmake_minimum_required(VERSION 3.1.0)
project(Sims2GameWriter)

# Find other dependencies
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6Widgets REQUIRED)
find_package(Qt6LinguistTools NO_CMAKE_FIND_ROOT_PATH)
if (NOT GraphicsRulesMaker_FULL_BUILD)
    find_package(GraphicsRulesMaker REQUIRED)
endif (NOT GraphicsRulesMaker_FULL_BUILD)

include_directories(${GraphicsRulesMaker_INCLUDE_DIRS})

if (NOT DEFINED GraphicsRulesMaker_TRANSLATIONS_DIR)
    # This will put only this subproject's translations in a subdir within this project
    set(GraphicsRulesMaker_TRANSLATIONS_DIR "${PROJECT_SOURCE_DIR}/translations")
endif (NOT DEFINED GraphicsRulesMaker_TRANSLATIONS_DIR)

qt6_wrap_ui(GraphicsRulesMaker_Sims2_UIS sims2settings.ui)

SET(GraphicsRulesMaker_Sims2_SRCS
    sims2settings.cpp
    sims2gamewriter.cpp
    ${GraphicsRulesMaker_Sims2_UIS}
)

# Translations
if(${Qt6LinguistTools_FOUND})
    message(STATUS "Qt Linguist found - will set up translations")
    qt6_create_translation(GraphicsRulesMaker_Sims2_QMS
        ${CMAKE_CURRENT_SOURCE_DIR}
        "${GraphicsRulesMaker_TRANSLATIONS_DIR}/Sims2GameWriter_nl.ts"
        OPTIONS -source-language "en_US"
    )
endif(${Qt6LinguistTools_FOUND})

add_library(Sims2GameWriter MODULE ${GraphicsRulesMaker_Sims2_SRCS} ${GraphicsRulesMaker_Sims2_QMS})
set_target_properties(Sims2GameWriter PROPERTIES PREFIX "")
target_link_libraries(Sims2GameWriter Qt6::Widgets GraphicsRulesMaker)

# Determine relative plugin path for CPACK
file(RELATIVE_PATH PluginInstallDir "${CMAKE_INSTALL_PREFIX}" ${GraphicsRulesMaker_PLUGIN_PATH})

# Determine *.qm installation path
if (NOT DEFINED TRANSLATIONS_INSTALL_DIR)
    set(TRANSLATIONS_INSTALL_DIR ${PluginInstallDir})
endif(NOT DEFINED TRANSLATIONS_INSTALL_DIR)

install(TARGETS Sims2GameWriter
    RUNTIME DESTINATION ${PluginInstallDir}
    LIBRARY DESTINATION ${PluginInstallDir}
    COMPONENT sims2
)
install(
    FILES ${GraphicsRulesMaker_Sims2_QMS}
    DESTINATION ${TRANSLATIONS_INSTALL_DIR}
    COMPONENT sims2
)
