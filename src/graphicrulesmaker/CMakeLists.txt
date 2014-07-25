set(GraphicRulesMaker_SRCS
    videocarddatabase.cpp
    devicemodel.cpp
)

add_library(GraphicRulesMaker SHARED ${GraphicRulesMaker_SRCS})

target_link_libraries(GraphicRulesMaker Qt5::Widgets)
if(WIN32)
    target_link_libraries(GraphicRulesMaker ${DirectX_LIBRARIES})
endif(WIN32)

if(WIN32)
    install(TARGETS GraphicRulesMaker
        RUNTIME DESTINATION bin
        COMPONENT application
    )
    # Different component for installer
    install(TARGETS GraphicRulesMaker
        LIBRARY DESTINATION lib${LIB_SUFFIX}
        ARCHIVE DESTINATION lib${LIB_SUFFIX}
        COMPONENT dev
    )
else(WIN32)
    install(TARGETS GraphicRulesMaker
        LIBRARY DESTINATION lib${LIB_SUFFIX}
        ARCHIVE DESTINATION lib${LIB_SUFFIX}
        COMPONENT application
    )
endif(WIN32)

install(FILES
        "devicemodel.h"
        "videocarddatabase.h"
        "gamewriterinterface.h"
    DESTINATION include/graphicrulesmaker/
    COMPONENT dev
)
