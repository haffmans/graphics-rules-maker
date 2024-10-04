cmake_minimum_required(VERSION 3.24)
project(SimsCastawayStoriesGameWriter)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GraphicsRulesMaker REQUIRED)

qt_wrap_ui(GraphicsRulesMaker_SimsCastawayStories_UIS simscssettings.ui)

graphicsrulesmaker_add_plugin(SimsCastawayStoriesGameWriter
    simscssettings.cpp
    simscsgamewriter.cpp
    ${GraphicsRulesMaker_SimsCastawayStories_UIS}
)
graphicsrulesmaker_translations(SimsCastawayStoriesGameWriter LANGUAGES nl)

install(TARGETS SimsCastawayStoriesGameWriter
    RUNTIME DESTINATION ${GraphicsRulesMaker_INSTALL_PLUGIN_DIR}
    LIBRARY DESTINATION ${GraphicsRulesMaker_INSTALL_PLUGIN_DIR}
    COMPONENT simscastawaystories
)
install(
    FILES ${SimsCastawayStoriesGameWriter_TRANSLATIONS}
    DESTINATION ${GraphicsRulesMaker_INSTALL_TRANSLATIONS_DIR}
    COMPONENT simscastawaystories
)
