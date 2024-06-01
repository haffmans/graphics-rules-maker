cmake_minimum_required(VERSION 3.1.0)
project(SimCity4GameWriter)

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

qt6_wrap_ui(GraphicsRulesMaker_SimCity4_UIS simcity4settings.ui)

SET(GraphicsRulesMaker_SimCity4_SRCS
    simcity4settings.cpp
    simcity4gamewriter.cpp
    ${GraphicsRulesMaker_SimCity4_UIS}
)

# Translations
if(${Qt6LinguistTools_FOUND})
    message(STATUS "Qt Linguist found - will set up translations")
    qt6_create_translation(GraphicsRulesMaker_SimCity4_QMS
        ${CMAKE_CURRENT_SOURCE_DIR}
        "${CMAKE_CURRENT_SOURCE_DIR}/translations/SimCity4GameWriter_nl.ts"
        OPTIONS -source-language "en_US"
    )
endif(${Qt6LinguistTools_FOUND})

add_library(SimCity4GameWriter MODULE ${GraphicsRulesMaker_SimCity4_SRCS} ${GraphicsRulesMaker_SimCity4_QMS})
set_target_properties(SimCity4GameWriter PROPERTIES PREFIX "")
target_link_libraries(SimCity4GameWriter Qt6::Widgets GraphicsRulesMaker)

# Determine relative plugin path for CPACK
file(RELATIVE_PATH PluginInstallDir "${CMAKE_INSTALL_PREFIX}" ${GraphicsRulesMaker_PLUGIN_PATH})

# Determine *.qm installation path
if (NOT DEFINED TRANSLATIONS_INSTALL_DIR)
    set(TRANSLATIONS_INSTALL_DIR ${PluginInstallDir})
endif(NOT DEFINED TRANSLATIONS_INSTALL_DIR)

install(TARGETS SimCity4GameWriter
    RUNTIME DESTINATION ${PluginInstallDir}
    LIBRARY DESTINATION ${PluginInstallDir}
    COMPONENT simcity4
)
install(
    FILES ${GraphicsRulesMaker_SimCity4_QMS}
    DESTINATION ${TRANSLATIONS_INSTALL_DIR}
    COMPONENT simcity4
)
