
qt5_wrap_ui(GraphicRulesMaker_Sims2_UIS sims2settings.ui)

SET(GraphicRulesMaker_Sims2_SRCS
    sims2settings.cpp
    sims2gamewriter.cpp
    ${GraphicRulesMaker_Sims2_UIS}
)

add_library(Sims2GameWriter MODULE ${GraphicRulesMaker_Sims2_SRCS})
target_link_libraries(Sims2GameWriter Qt5::Widgets)

install(TARGETS Sims2GameWriter
    RUNTIME DESTINATION ${GraphicRulesMaker_PLUGIN_INSTALL_DIR}
    LIBRARY DESTINATION ${GraphicRulesMaker_PLUGIN_INSTALL_DIR}
)
