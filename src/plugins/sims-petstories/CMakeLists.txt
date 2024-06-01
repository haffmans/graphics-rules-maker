cmake_minimum_required(VERSION 3.24)
project(SimsPetStoriesGameWriter)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GraphicsRulesMaker REQUIRED)

qt_wrap_ui(GraphicsRulesMaker_SimsPetStories_UIS simspssettings.ui)

graphicsrulesmaker_add_plugin(SimsPetStoriesGameWriter
    simspssettings.cpp
    simspsgamewriter.cpp
    ${GraphicsRulesMaker_SimsPetStories_UIS}
)
graphicsrulesmaker_translations(SimsPetStoriesGameWriter LANGUAGES nl)

install(TARGETS SimsPetStoriesGameWriter
    RUNTIME DESTINATION ${GraphicsRulesMaker_INSTALL_PLUGIN_DIR}
    LIBRARY DESTINATION ${GraphicsRulesMaker_INSTALL_PLUGIN_DIR}
    COMPONENT simspetstories
)
install(
    FILES ${SimsPetStoriesGameWriter_TRANSLATIONS}
    DESTINATION ${GraphicsRulesMaker_INSTALL_TRANSLATIONS_DIR}
    COMPONENT simspetstories
)
