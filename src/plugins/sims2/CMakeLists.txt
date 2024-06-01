cmake_minimum_required(VERSION 3.24)
project(Sims2GameWriter)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GraphicsRulesMaker REQUIRED)

qt6_wrap_ui(GraphicsRulesMaker_Sims2_UIS sims2settings.ui)

graphicsrulesmaker_add_plugin(Sims2GameWriter
    sims2settings.cpp
    sims2gamewriter.cpp
    ${GraphicsRulesMaker_Sims2_UIS}
)
graphicsrulesmaker_translations(Sims2GameWriter LANGUAGES nl)

install(TARGETS Sims2GameWriter
    RUNTIME DESTINATION ${GraphicsRulesMaker_INSTALL_PLUGIN_DIR}
    LIBRARY DESTINATION ${GraphicsRulesMaker_INSTALL_PLUGIN_DIR}
    COMPONENT sims2
)
install(
    FILES ${Sims2GameWriter_TRANSLATIONS}
    DESTINATION ${GraphicsRulesMaker_INSTALL_TRANSLATIONS_DIR}
    COMPONENT sims2
)
