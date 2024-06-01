cmake_minimum_required(VERSION 3.24)
project(SimCity4GameWriter)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GraphicsRulesMaker REQUIRED)

qt6_wrap_ui(GraphicsRulesMaker_SimCity4_UIS simcity4settings.ui)

graphicsrulesmaker_add_plugin(SimCity4GameWriter
    simcity4settings.cpp
    simcity4gamewriter.cpp
    ${GraphicsRulesMaker_SimCity4_UIS}
)
graphicsrulesmaker_translations(SimCity4GameWriter LANGUAGES nl)

install(TARGETS SimCity4GameWriter
    RUNTIME DESTINATION ${GraphicsRulesMaker_INSTALL_PLUGIN_DIR}
    LIBRARY DESTINATION ${GraphicsRulesMaker_INSTALL_PLUGIN_DIR}
    COMPONENT simcity4
)
install(
    FILES ${SimCity4GameWriter_TRANSLATIONS}
    DESTINATION ${GraphicsRulesMaker_INSTALL_TRANSLATIONS_DIR}
    COMPONENT simcity4
)
