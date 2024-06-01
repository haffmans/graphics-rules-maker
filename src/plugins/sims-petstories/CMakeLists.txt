cmake_minimum_required(VERSION 3.24)
project(SimsPetStoriesGameWriter)

# Find other dependencies
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6Widgets REQUIRED)
find_package(Qt6LinguistTools NO_CMAKE_FIND_ROOT_PATH)
find_package(GraphicsRulesMaker REQUIRED)

include_directories(${GraphicsRulesMaker_INCLUDE_DIRS})

qt6_wrap_ui(GraphicsRulesMaker_SimsPetStories_UIS simspssettings.ui)

SET(GraphicsRulesMaker_SimsPetStories_SRCS
    simspssettings.cpp
    simspsgamewriter.cpp
    ${GraphicsRulesMaker_SimsPetStories_UIS}
)

# Translations
if(${Qt6LinguistTools_FOUND})
    message(STATUS "Qt Linguist found - will set up translations")
    qt6_create_translation(GraphicsRulesMaker_SimsPetStories_QMS
        ${CMAKE_CURRENT_SOURCE_DIR}
        "${CMAKE_CURRENT_SOURCE_DIR}/translations/SimsPetStoriesGameWriter_nl.ts"
        OPTIONS -source-language "en_US"
    )
endif(${Qt6LinguistTools_FOUND})

add_library(SimsPetStoriesGameWriter MODULE ${GraphicsRulesMaker_SimsPetStories_SRCS} ${GraphicsRulesMaker_SimsPetStories_QMS})
set_target_properties(SimsPetStoriesGameWriter PROPERTIES PREFIX "")
target_link_libraries(SimsPetStoriesGameWriter Qt6::Widgets GraphicsRulesMaker)

install(TARGETS SimsPetStoriesGameWriter
    RUNTIME DESTINATION ${GraphicsRulesMaker_INSTALL_PLUGIN_DIR}
    LIBRARY DESTINATION ${GraphicsRulesMaker_INSTALL_PLUGIN_DIR}
    COMPONENT simspetstories
)
install(
    FILES ${GraphicsRulesMaker_SimsPetStories_QMS}
    DESTINATION ${GraphicsRulesMaker_INSTALL_TRANSLATIONS_DIR}
    COMPONENT simspetstories
)
