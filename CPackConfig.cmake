set(CPACK_PACKAGE_NAME "GraphicRulesMaker")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Graphic Rules Maker")
set(CPACK_PACKAGE_VENDOR "SimsNetwork.com")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Readme.txt")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Graphic Rules Maker")

# Components: application, dev, sims2
set(CPACK_COMPONENTS_ALL application dev sims2)

set(CPACK_COMPONENT_APPLICATION_DISPLAY_NAME "Graphic Rules Maker Application")
set(CPACK_COMPONENT_APPLICATION_DESCRIPTION "The application and all required files.")
set(CPACK_COMPONENT_APPLICATION_REQUIRED 1)
set(CPACK_COMPONENT_APPLICATION_INSTALL_TYPES Default Dev)
set(CPACK_COMPONENT_APPLICATION_GROUP App)

set(CPACK_COMPONENT_DEV_DISPLAY_NAME "Development files")
set(CPACK_COMPONENT_DEV_DESCRIPTION "Files and headers required to develop new writer plugins.")
set(CPACK_COMPONENT_DEV_INSTALL_TYPES Dev)
set(CPACK_COMPONENT_DEV_DISABLED 1)
set(CPACK_COMPONENT_DEV_GROUP App)

set(CPACK_COMPONENT_SIMS2_DISPLAY_NAME "Sims 2")
set(CPACK_COMPONENT_SIMS2_DESCRIPTION "Sims 2 plugin")
set(CPACK_COMPONENT_SIMS2_REQUIRED 1)
set(CPACK_COMPONENT_SIMS2_INSTALL_TYPES Default Dev)
set(CPACK_COMPONENT_SIMS2_GROUP Plugins)
set(CPACK_COMPONENT_SIMS2_DEPENDS application)

set(CPACK_COMPONENT_GROUP_APP_DISPLAY_NAME "Graphic Rules Maker")
set(CPACK_COMPONENT_GROUP_APP_DESCRIPTION "The application itself.")
set(CPACK_COMPONENT_GROUP_APP_EXPANDED 0)

set(CPACK_COMPONENT_GROUP_PLUGINS_DISPLAY_NAME "Plugins")
set(CPACK_COMPONENT_GROUP_PLUGINS_DESCRIPTION "Game plugins")
set(CPACK_COMPONENT_GROUP_PLUGINS_EXPANDED 1)

set(CPACK_INSTALL_TYPE_DEFAULT_DISPLAY_NAME "Default Installation")
set(CPACK_INSTALL_TYPE_DEV_DISPLAY_NAME "Developers")

if(WIN32 AND NOT UNIX)
    set(CPACK_GENERATOR "NSIS;ZIP")

    # There is a bug in NSI that does not handle full unix paths properly. Make
    # sure there is at least one set of four (4) backslashes.
    set(CPACK_PACKAGE_ICON "${PROJECT_BINARY_DIR}\\\\header.bmp")
    set(CPACK_NSIS_MUI_ICON "${PROJECT_BINARY_DIR}\\\\src\\\\graphicrulesmakerui\\\\icon.ico")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\GraphicRulesMakerUI.exe")
    set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY}")
    set(CPACK_NSIS_HELP_LINK "http://www.simsnetwork.com")
    set(CPACK_NSIS_URL_INFO_ABOUT "http://www.simsnetwork.com")
    set(CPACK_NSIS_CONTACT "wouter@simply-life.net")
    set(CPACK_NSIS_MODIFY_PATH OFF)
    set(CPACK_NSIS_CREATE_ICONS_EXTRA "!insertmacro CreateInternetShortcut '\$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\SimsNetwork.url' 'http://www.simsnetwork.com' '$INSTDIR\\\\simsnetwork.ico' 0")
    set(CPACK_NSIS_DELETE_ICONS_EXTRA "Delete '\$SMPROGRAMS\\\\$MUI_TEMP\\\\SimsNetwork.url'")
    set(CPACK_NSIS_MUI_FINISHPAGE_RUN "GraphicRulesMakerUI.exe")
else(WIN32 AND NOT UNIX)
    set(CPACK_STRIP_FILES "bin/GraphicRulesMakerUI")
    set(CPACK_SOURCE_STRIP_FILES "")
endif(WIN32 AND NOT UNIX)
set(CPACK_PACKAGE_EXECUTABLES "GraphicRulesMakerUI" "Graphic Rules Maker")