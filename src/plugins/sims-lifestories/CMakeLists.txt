cmake_minimum_required(VERSION 3.24)
project(SimsLifeStoriesGameWriter)

# Find other dependencies
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6Widgets REQUIRED)
find_package(Qt6LinguistTools NO_CMAKE_FIND_ROOT_PATH)
find_package(GraphicsRulesMaker REQUIRED)

include_directories(${GraphicsRulesMaker_INCLUDE_DIRS})

qt6_wrap_ui(GraphicsRulesMaker_SimsLifeStories_UIS simslssettings.ui)

SET(GraphicsRulesMaker_SimsLifeStories_SRCS
    simslssettings.cpp
    simslsgamewriter.cpp
    ${GraphicsRulesMaker_SimsLifeStories_UIS}
)

# Translations
if(${Qt6LinguistTools_FOUND})
    message(STATUS "Qt Linguist found - will set up translations")
    qt6_create_translation(GraphicsRulesMaker_SimsLifeStories_QMS
        ${CMAKE_CURRENT_SOURCE_DIR}
        "${CMAKE_CURRENT_SOURCE_DIR}/translations/SimsLifeStoriesGameWriter_nl.ts"
        OPTIONS -source-language "en_US"
    )
endif(${Qt6LinguistTools_FOUND})

add_library(SimsLifeStoriesGameWriter MODULE ${GraphicsRulesMaker_SimsLifeStories_SRCS} ${GraphicsRulesMaker_SimsLifeStories_QMS})
set_target_properties(SimsLifeStoriesGameWriter PROPERTIES PREFIX "")
target_link_libraries(SimsLifeStoriesGameWriter Qt6::Widgets GraphicsRulesMaker)

install(TARGETS SimsLifeStoriesGameWriter
    RUNTIME DESTINATION ${GraphicsRulesMaker_INSTALL_PLUGIN_DIR}
    LIBRARY DESTINATION ${GraphicsRulesMaker_INSTALL_PLUGIN_DIR}
    COMPONENT simslifestories
)
install(
    FILES ${GraphicsRulesMaker_SimsLifeStories_QMS}
    DESTINATION ${GraphicsRulesMaker_INSTALL_TRANSLATIONS_DIR}
    COMPONENT simslifestories
)
