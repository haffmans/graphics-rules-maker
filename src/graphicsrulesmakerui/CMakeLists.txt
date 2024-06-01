cmake_minimum_required(VERSION 3.24)
project(GraphicsRulesMakerUi)

# Find other dependencies
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6Widgets REQUIRED)
find_package(Qt6LinguistTools NO_CMAKE_FIND_ROOT_PATH)
find_package(GraphicsRulesMaker REQUIRED)

find_package(ImageMagick REQUIRED COMPONENTS convert)

# Create icon from icon.png
add_custom_command(OUTPUT "icon-48.png" "icon-32.png" "icon-16.png"
    COMMAND ${IMAGEMAGICK_CONVERT_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/icon.png" -resize 48x48 "${CMAKE_CURRENT_BINARY_DIR}/icon-48.png"
    COMMAND ${IMAGEMAGICK_CONVERT_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/icon.png" -resize 32x32 "${CMAKE_CURRENT_BINARY_DIR}/icon-32.png"
    COMMAND ${IMAGEMAGICK_CONVERT_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/icon.png" -resize 16x16 "${CMAKE_CURRENT_BINARY_DIR}/icon-16.png"
    COMMENT "Resizing icon"
    MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/icon.png"
)

add_custom_command(OUTPUT "icon.ico"
    COMMAND ${IMAGEMAGICK_CONVERT_EXECUTABLE}
        "${CMAKE_CURRENT_SOURCE_DIR}/icon.png"
        "icon-48.png"
        "icon-32.png"
        "icon-16.png"
        "icon.ico"
    COMMENT "Creating final icon"
    MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/icon.png"
    DEPENDS "icon-48.png" "icon-32.png" "icon-16.png"
)

set(GraphicsRulesMaker_Ico "${CMAKE_CURRENT_BINARY_DIR}/icon.ico")

qt6_add_resources(GraphicsRulesMaker_IconRCC
    icon.qrc
)

configure_file(graphicsrulesmakerico.rc.in
    ${CMAKE_CURRENT_BINARY_DIR}/graphicsrulesmakerico.rc
)
set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/graphicsrulesmakerico.rc APPEND PROPERTY OBJECT_DEPENDS ${GraphicsRulesMaker_Ico})

add_library(GraphicsRulesMaker_Icon OBJECT
    ${CMAKE_CURRENT_BINARY_DIR}/graphicsrulesmakerico.rc
    ${GraphicsRulesMaker_IconRCC}
)

qt6_wrap_ui(GraphicsRulesMakerUi_UIS
    mainwindow.ui
    manualsaveconfirmationbox.ui
    aboutdialog.ui
)

# GraphicsRulesMaker UI
set(GraphicsRulesMakerUi_SRCS
    main.cpp
    mainwindow.cpp
    manualsaveconfirmationbox.cpp
    aboutdialog.cpp
    ${GraphicsRulesMakerUi_UIS}
    ${GraphicsRulesMakerUi_RCCS}
    $<TARGET_OBJECTS:GraphicsRulesMaker_Icon>
)

# Translations
if(${Qt6LinguistTools_FOUND})
    message(STATUS "Qt Linguist found - will set up translations")
    qt6_create_translation(GraphicsRulesMakerUi_QMS
        ${CMAKE_CURRENT_SOURCE_DIR}
        "${CMAKE_CURRENT_SOURCE_DIR}/translations/GraphicsRulesMakerUi_nl.ts"
        OPTIONS -source-language "en_US"
    )
endif(${Qt6LinguistTools_FOUND})

# Set some Win32 Specific Settings
IF(WIN32)
    SET(GUI_TYPE WIN32)
ENDIF(WIN32)
# Set some Apple MacOS Specific settings
IF (APPLE)
    SET(GUI_TYPE MACOSX_BUNDLE)
ENDIF (APPLE)

add_executable(GraphicsRulesMakerUi ${GUI_TYPE} ${GraphicsRulesMakerUi_SRCS} ${GraphicsRulesMakerUi_QMS})
target_link_libraries(GraphicsRulesMakerUi PUBLIC GraphicsRulesMaker Qt6::Widgets)

install(TARGETS GraphicsRulesMakerUi
    RUNTIME DESTINATION bin
    COMPONENT application
)
install(
    FILES ${GraphicsRulesMakerUi_QMS}
    DESTINATION ${GraphicsRulesMaker_INSTALL_TRANSLATIONS_DIR}
    COMPONENT application
)

if(WIN32)
    include(InstallRequiredSystemLibraries)

    # Install dependencies (Qt) for Windows
    get_target_property(Qt6Core_Location Qt6::Core LOCATION)
    get_filename_component(Qt6Core_BinDir "${Qt6Core_Location}" DIRECTORY)

    if (TARGET Qt6::QWindowsIntegrationPlugin)
        # For MSVC (as generator), there's no active build config; the configuration is determined
        # at build time instead of configure time. So: if there are imported configurations, use a
        # generator expression to have the correct file deployed at install time; otherwise fallback
        # to the default.
        get_target_property(_qwindows_configs Qt6::QWindowsIntegrationPlugin IMPORTED_CONFIGURATIONS)
        if (_qwindows_configs)
            message(STATUS "Qt6 Windows integration plugin: FOUND, using imported location")
            set(_plugins
                "$<TARGET_FILE:Qt6::QWindowsIntegrationPlugin>"
            )
        else()
            message(STATUS "Qt6 Windows integration plugin: FOUND, using default path")
            set(_plugins
                "platforms/qwindows.dll"
            )
        endif()

    else()
        # Integration plugin target not defined - just hardcode it
        message(STATUS "Windows integration plugin not found; assuming default path")
        set(_plugins
            "platforms/qwindows.dll"
        )
    endif()

    set(_pluginSourcePaths
        "${QT6_INSTALL_PREFIX}/lib/qt6/plugins"
        "${QT6_INSTALL_PREFIX}/plugins"
    )

    foreach(_basePath ${_pluginSourcePaths})
        if(EXISTS ${_basePath})
            set(_pluginBasePath ${_basePath})
        endif()
    endforeach()

    if (NOT _pluginBasePath)
        message(FATAL_ERROR "Could not detect Qt plugin base directory from: ${QT6_INSTALL_PREFIX}")
    endif()

    foreach(_plugin ${_plugins})
        # Some broken up generator expressions (for readability)
        set(_plugin_is_relative "$<PATH:IS_RELATIVE,${_plugin}>")
        set(_plugin_made_relative "$<PATH:RELATIVE_PATH,${_plugin},${_pluginBasePath}>")
        set(_plugin_relative "$<IF:${_plugin_is_relative},${_plugin},${_plugin_made_relative}>")
        set(_plugin_relative_dir "$<PATH:REMOVE_FILENAME,${_plugin_relative}>")

        install(FILES "$<PATH:ABSOLUTE_PATH,${_plugin},${_pluginBasePath}>"
                DESTINATION "bin/${_plugin_relative_dir}"
                COMPONENT application
        )
    endforeach()

    # Create bundle installation for Windows

    # "CMAKE_INSTALL_PREFIX" must be resolved during CPack time, so the $ needs to be escaped!
    set(BundleApp "\${CMAKE_INSTALL_PREFIX}/bin/GraphicsRulesMakerUi${CMAKE_EXECUTABLE_SUFFIX}")

    set(BundleLibraryDirs
        "\${CMAKE_INSTALL_PREFIX}/bin/"
        "\${CMAKE_INSTALL_PREFIX}/bin/gamewriters"
        ${Qt6Core_BinDir}
        ${CMAKE_FIND_ROOT_PATH}/bin
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
