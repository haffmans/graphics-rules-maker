
# Define a new plugin for GraphicsRulesMaker
# Usage:
#    graphicsrulesmaker_add_plugin(<target name> [<source file> ...])
#
# The created target will be set up to link to the GraphicsRulesMaker core library.
#
macro(graphicsrulesmaker_add_plugin TARGET)
    add_library(${TARGET} MODULE ${ARGN})
    # avoid `lib` prefix for plugins
    set_target_properties(${TARGET} PROPERTIES PREFIX "")
    target_link_libraries(${TARGET} PUBLIC GraphicsRulesMaker Qt6::Widgets)
endmacro()
