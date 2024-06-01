cmake_minimum_required(VERSION 3.24)
project(Sims2BodyShopGameWriter)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GraphicsRulesMaker REQUIRED)

qt6_wrap_ui(GraphicsRulesMaker_Sims2BodyShop_UIS sims2bodyshopsettings.ui)

graphicsrulesmaker_add_plugin(Sims2BodyShopGameWriter
    sims2bodyshopsettings.cpp
    sims2bodyshopgamewriter.cpp
    ${GraphicsRulesMaker_Sims2BodyShop_UIS}
)
graphicsrulesmaker_translations(Sims2BodyShopGameWriter LANGUAGES nl)

install(TARGETS Sims2BodyShopGameWriter
    RUNTIME DESTINATION ${GraphicsRulesMaker_INSTALL_PLUGIN_DIR}
    LIBRARY DESTINATION ${GraphicsRulesMaker_INSTALL_PLUGIN_DIR}
    COMPONENT sims2
)
install(
    FILES ${Sims2BodyShopGameWriter_TRANSLATIONS}
    DESTINATION ${GraphicsRulesMaker_INSTALL_TRANSLATIONS_DIR}
    COMPONENT sims2
)
