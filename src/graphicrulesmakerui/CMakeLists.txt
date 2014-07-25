
qt5_wrap_ui(GraphicRulesMaker_UIS
    mainwindow.ui
    manualsaveconfirmationbox.ui
    aboutdialog.ui
)

# GraphicRulesMaker UI
set(GraphicRulesMakerUi_SRCS
    main.cpp
    mainwindow.cpp
    manualsaveconfirmationbox.cpp
    aboutdialog.cpp
    gamewriterfactory.cpp
    ${GraphicRulesMaker_UIS}
)

# Set some Win32 Specific Settings
IF(WIN32)
    SET(GUI_TYPE WIN32)
ENDIF(WIN32)
# Set some Apple MacOS Specific settings
IF (APPLE)
    SET(GUI_TYPE MACOSX_BUNDLE)
ENDIF (APPLE)

add_executable(GraphicRulesMakerUi ${GUI_TYPE} ${GraphicRulesMakerUi_SRCS})
target_link_libraries(GraphicRulesMakerUi GraphicRulesMaker Qt5::Widgets)

if(WIN32)
    target_link_libraries(GraphicRulesMakerUi ${DirectX_LIBRARIES})
endif(WIN32)

install(TARGETS GraphicRulesMakerUi
    RUNTIME DESTINATION bin
    COMPONENT application
)
