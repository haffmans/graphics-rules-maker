cmake_minimum_required(VERSION 3.24)
project(SimCity4GameWriter)

# Find other dependencies
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6Widgets REQUIRED)
find_package(Qt6LinguistTools NO_CMAKE_FIND_ROOT_PATH)
find_package(GraphicsRulesMaker REQUIRED)

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
target_link_libraries(SimCity4GameWriter PUBLIC Qt6::Widgets GraphicsRulesMaker)

install(TARGETS SimCity4GameWriter
    RUNTIME DESTINATION ${GraphicsRulesMaker_INSTALL_PLUGIN_DIR}
    LIBRARY DESTINATION ${GraphicsRulesMaker_INSTALL_PLUGIN_DIR}
    COMPONENT simcity4
)
install(
    FILES ${GraphicsRulesMaker_SimCity4_QMS}
    DESTINATION ${GraphicsRulesMaker_INSTALL_TRANSLATIONS_DIR}
    COMPONENT simcity4
)
