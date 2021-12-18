cmake_minimum_required(VERSION 3.19)
project(GraphicsRulesMakerUi)

# Find other dependencies
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5LinguistTools)
if (NOT GraphicsRulesMaker_FULL_BUILD)
    find_package(GraphicsRulesMaker REQUIRED)
endif (NOT GraphicsRulesMaker_FULL_BUILD)
find_package(ImageMagick REQUIRED COMPONENTS convert)

if (NOT DEFINED GraphicsRulesMaker_TRANSLATIONS_DIR)
    # This will put only this subproject's translations in a subdir within this project
    set(GraphicsRulesMaker_TRANSLATIONS_DIR "${PROJECT_SOURCE_DIR}/translations")
endif (NOT DEFINED GraphicsRulesMaker_TRANSLATIONS_DIR)
if(WIN32 AND NOT CYGWIN)
    set(TRANSLATIONS_INSTALL_DIR share/)
else(WIN32 AND NOT CYGWIN)
    set(TRANSLATIONS_INSTALL_DIR share/graphicsrulesmaker/)
endif(WIN32 AND NOT CYGWIN)

include_directories(${GraphicsRulesMaker_INCLUDE_DIRS})

qt5_wrap_ui(GraphicsRulesMakerUi_UIS
    mainwindow.ui
    manualsaveconfirmationbox.ui
    aboutdialog.ui
)

qt5_add_resources(GraphicsRulesMakerUi_RCCS
    icon.qrc
)

# Create icon from icon.png
add_custom_command(OUTPUT "icon-48.png" "icon-32.png" "icon-16.png"
    COMMAND ${IMAGEMAGICK_CONVERT_EXECUTABLE} "${PROJECT_SOURCE_DIR}/icon.png" -resize 48x48 "${PROJECT_BINARY_DIR}/icon-48.png"
    COMMAND ${IMAGEMAGICK_CONVERT_EXECUTABLE} "${PROJECT_SOURCE_DIR}/icon.png" -resize 32x32 "${PROJECT_BINARY_DIR}/icon-32.png"
    COMMAND ${IMAGEMAGICK_CONVERT_EXECUTABLE} "${PROJECT_SOURCE_DIR}/icon.png" -resize 16x16 "${PROJECT_BINARY_DIR}/icon-16.png"
    COMMENT "Resizing icon"
    MAIN_DEPENDENCY "${PROJECT_SOURCE_DIR}/icon.png"
)

add_custom_command(OUTPUT "icon.ico"
    COMMAND ${IMAGEMAGICK_CONVERT_EXECUTABLE}
        "${PROJECT_SOURCE_DIR}/icon.png"
        "icon-48.png"
        "icon-32.png"
        "icon-16.png"
        "icon.ico"
    COMMENT "Creating final icon"
    MAIN_DEPENDENCY "${PROJECT_SOURCE_DIR}/icon.png"
    DEPENDS "icon-48.png" "icon-32.png" "icon-16.png"
)

set(GraphicsRulesMakerUi_Ico "${PROJECT_BINARY_DIR}/icon.ico")

configure_file(graphicsrulesmakerui.rc.in
    ${PROJECT_BINARY_DIR}/graphicsrulesmakerui.rc)

# GraphicsRulesMaker UI
set(GraphicsRulesMakerUi_SRCS
    main.cpp
    mainwindow.cpp
    manualsaveconfirmationbox.cpp
    aboutdialog.cpp
    ${GraphicsRulesMakerUi_UIS}
    ${GraphicsRulesMakerUi_RCCS}
    ${GraphicsRulesMakerUi_Ico}
    ${PROJECT_BINARY_DIR}/graphicsrulesmakerui.rc
)

# Translations
if(${Qt5LinguistTools_FOUND})
    message(STATUS "Qt Linguist found - will set up translations")
    qt5_create_translation(GraphicsRulesMakerUi_QMS
        ${CMAKE_CURRENT_SOURCE_DIR}
        "${GraphicsRulesMaker_TRANSLATIONS_DIR}/GraphicsRulesMakerUi_nl.ts"
        OPTIONS -source-language "en_US"
    )
endif(${Qt5LinguistTools_FOUND})

# Set some Win32 Specific Settings
IF(WIN32)
    SET(GUI_TYPE WIN32)
ENDIF(WIN32)
# Set some Apple MacOS Specific settings
IF (APPLE)
    SET(GUI_TYPE MACOSX_BUNDLE)
ENDIF (APPLE)

add_executable(GraphicsRulesMakerUi ${GUI_TYPE} ${GraphicsRulesMakerUi_SRCS} ${GraphicsRulesMakerUi_QMS})
target_link_libraries(GraphicsRulesMakerUi GraphicsRulesMaker Qt5::Widgets)

if(WIN32)
    target_link_libraries(GraphicsRulesMakerUi ${DirectX_LIBRARIES})
endif(WIN32)

install(TARGETS GraphicsRulesMakerUi
    RUNTIME DESTINATION bin
    COMPONENT application
)
install(
    FILES ${GraphicsRulesMakerUi_QMS}
    DESTINATION ${TRANSLATIONS_INSTALL_DIR}
    COMPONENT application
)

if(WIN32)
    include(InstallRequiredSystemLibraries)

    # Install dependencies (Qt) for Windows
    get_target_property(Qt5Core_Location Qt5::Core LOCATION)
    get_filename_component(Qt5Core_BinDir "${Qt5Core_Location}" DIRECTORY)

    set(_plugins
        "platforms/qwindows.dll"
    )

    foreach(_plugin ${_plugins})
        message(STATUS "Plugin: ${_plugin}")
        get_filename_component(_plugin_dir ${_plugin} DIRECTORY)

        install(FILES ${_qt5Core_install_prefix}/lib/qt/plugins/${_plugin}
                DESTINATION bin/${_plugin_dir}
                COMPONENT application
        )
    endforeach()

    # Create bundle installation for Windows

    # "CMAKE_INSTALL_PREFIX" must be resolved during CPack time, so the $ needs to be escaped!
    set(BundleApp "\${CMAKE_INSTALL_PREFIX}/bin/GraphicsRulesMakerUi${CMAKE_EXECUTABLE_SUFFIX}")

    set(BundleLibraryDirs
        "\${CMAKE_INSTALL_PREFIX}/bin/"
        "\${CMAKE_INSTALL_PREFIX}/bin/gamewriters"
        ${Qt5Core_BinDir}
    )

    if (NOT "${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Windows")
        # Cross compiling; force using objdump to verify prerequisites
        set(gp_tool "objdump")
    endif ()

    # Generate code to fixup bundle during install; installed plugins will be globbed during this step
    install(CODE "
        set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
        include(Win32CrossBundle)

        # Keep binary dir for Win32CrossBundle
        set(PROJECT_BINARY_DIR ${PROJECT_BINARY_DIR})

        # Force system correctly to work around issues with BundleUtilities trying to get the RPATH
        # for Windows DLLs (using file(READ_ELF)).
        set(UNIX ${UNIX})
        set(WIN32 ${WIN32})

        # Force gp_tool in cross-compile context to objdump
        set(gp_tool \"${gp_tool}\")

        if (NOT \$ENV{DESTDIR} STREQUAL \"\")
            message(STATUS \"Applying destdir \$ENV{DESTDIR}\")
            file(REAL_PATH \"\$ENV{DESTDIR}\" DESTDIR)
            set(DESTDIR \"\${DESTDIR}/\")
        endif()

        file(GLOB_RECURSE BundlePlugins
          \"\${DESTDIR}\${CMAKE_INSTALL_PREFIX}/bin/decoders/*${CMAKE_SHARED_LIBRARY_SUFFIX}\"
          \"\${DESTDIR}\${CMAKE_INSTALL_PREFIX}/bin/views/*${CMAKE_SHARED_LIBRARY_SUFFIX}\"
          \"\${DESTDIR}\${CMAKE_INSTALL_PREFIX}/bin/platform/*${CMAKE_SHARED_LIBRARY_SUFFIX}\"
          \"\${DESTDIR}\${CMAKE_INSTALL_PREFIX}/bin/imageformats/*${CMAKE_SHARED_LIBRARY_SUFFIX}\"
          \"\${DESTDIR}\${CMAKE_INSTALL_PREFIX}/bin/libEGL.dll\"
        )
        include(BundleUtilities)
        fixup_bundle(\"\${DESTDIR}${BundleApp}\" \"\${BundlePlugins}\" \"${BundleLibraryDirs}\")
    " COMPONENT application)
endif(WIN32)
