
qt5_wrap_ui(GraphicRulesMaker_Sims2_UIS sims2settings.ui)

SET(GraphicRulesMaker_Sims2_SRCS
    sims2settings.cpp
    sims2gamewriter.cpp
    ${GraphicRulesMaker_Sims2_UIS}
)

add_library(Sims2GameWriter MODULE ${GraphicRulesMaker_Sims2_SRCS})
set_target_properties(Sims2GameWriter PROPERTIES PREFIX "")
target_link_libraries(Sims2GameWriter Qt5::Widgets GraphicRulesMaker)

install(TARGETS Sims2GameWriter
    RUNTIME DESTINATION ${PLUGIN_INSTALL_DIR}
    LIBRARY DESTINATION ${PLUGIN_INSTALL_DIR}
    COMPONENT sims2
)
