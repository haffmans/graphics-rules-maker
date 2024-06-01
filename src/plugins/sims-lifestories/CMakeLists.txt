cmake_minimum_required(VERSION 3.24)
project(SimsLifeStoriesGameWriter)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GraphicsRulesMaker REQUIRED)

qt6_wrap_ui(GraphicsRulesMaker_SimsLifeStories_UIS simslssettings.ui)

graphicsrulesmaker_add_plugin(SimsLifeStoriesGameWriter
    simslssettings.cpp
    simslsgamewriter.cpp
    ${GraphicsRulesMaker_SimsLifeStories_UIS}
)
graphicsrulesmaker_translations(SimsLifeStoriesGameWriter LANGUAGES nl)

install(TARGETS SimsLifeStoriesGameWriter
    RUNTIME DESTINATION ${GraphicsRulesMaker_INSTALL_PLUGIN_DIR}
    LIBRARY DESTINATION ${GraphicsRulesMaker_INSTALL_PLUGIN_DIR}
    COMPONENT simslifestories
)
install(
    FILES ${SimsLifeStoriesGameWriter_TRANSLATIONS}
    DESTINATION ${GraphicsRulesMaker_INSTALL_TRANSLATIONS_DIR}
    COMPONENT simslifestories
)
