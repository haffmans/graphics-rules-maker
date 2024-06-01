cmake_minimum_required(VERSION 3.24)
project(Sims2GameWriter)

# Find other dependencies
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6Widgets REQUIRED)
find_package(Qt6LinguistTools NO_CMAKE_FIND_ROOT_PATH)
find_package(GraphicsRulesMaker REQUIRED)

include_directories(${GraphicsRulesMaker_INCLUDE_DIRS})

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
        "${CMAKE_CURRENT_SOURCE_DIR}/translations/Sims2GameWriter_nl.ts"
        OPTIONS -source-language "en_US"
    )
endif(${Qt6LinguistTools_FOUND})

add_library(Sims2GameWriter MODULE ${GraphicsRulesMaker_Sims2_SRCS} ${GraphicsRulesMaker_Sims2_QMS})
set_target_properties(Sims2GameWriter PROPERTIES PREFIX "")
target_link_libraries(Sims2GameWriter Qt6::Widgets GraphicsRulesMaker)

install(TARGETS Sims2GameWriter
    RUNTIME DESTINATION ${GraphicsRulesMaker_PLUGIN_PATH}
    LIBRARY DESTINATION ${GraphicsRulesMaker_PLUGIN_PATH}
    COMPONENT sims2
)
install(
    FILES ${GraphicsRulesMaker_Sims2_QMS}
    DESTINATION ${GraphicsRulesMaker_TRANSLATIONS_PATH}
    COMPONENT sims2
)
