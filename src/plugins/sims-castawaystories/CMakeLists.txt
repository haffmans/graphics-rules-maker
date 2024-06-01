cmake_minimum_required(VERSION 3.24)
project(SimsCastawayStoriesGameWriter)

# Find other dependencies
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6Widgets REQUIRED)
find_package(Qt6LinguistTools NO_CMAKE_FIND_ROOT_PATH)
find_package(GraphicsRulesMaker REQUIRED)

include_directories(${GraphicsRulesMaker_INCLUDE_DIRS})

qt6_wrap_ui(GraphicsRulesMaker_SimsCastawayStories_UIS simscssettings.ui)

SET(GraphicsRulesMaker_SimsCastawayStories_SRCS
    simscssettings.cpp
    simscsgamewriter.cpp
    ${GraphicsRulesMaker_SimsCastawayStories_UIS}
)

# Translations
if(${Qt6LinguistTools_FOUND})
    message(STATUS "Qt Linguist found - will set up translations")
    qt6_create_translation(GraphicsRulesMaker_SimsCastawayStories_QMS
        ${CMAKE_CURRENT_SOURCE_DIR}
        "${CMAKE_CURRENT_SOURCE_DIR}/translations/SimsCastawayStoriesGameWriter_nl.ts"
        OPTIONS -source-language "en_US"
    )
endif(${Qt6LinguistTools_FOUND})

add_library(SimsCastawayStoriesGameWriter MODULE ${GraphicsRulesMaker_SimsCastawayStories_SRCS} ${GraphicsRulesMaker_SimsCastawayStories_QMS})
set_target_properties(SimsCastawayStoriesGameWriter PROPERTIES PREFIX "")
target_link_libraries(SimsCastawayStoriesGameWriter Qt6::Widgets GraphicsRulesMaker)

install(TARGETS SimsCastawayStoriesGameWriter
    RUNTIME DESTINATION ${GraphicsRulesMaker_PLUGIN_PATH}
    LIBRARY DESTINATION ${GraphicsRulesMaker_PLUGIN_PATH}
    COMPONENT simscastawaystories
)
install(
    FILES ${GraphicsRulesMaker_SimsCastawayStories_QMS}
    DESTINATION ${GraphicsRulesMaker_TRANSLATIONS_PATH}
    COMPONENT simscastawaystories
)
