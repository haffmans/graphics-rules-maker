cmake_minimum_required(VERSION 3.1.0)
project(GraphicsRulesMakerUi)

# Find other dependencies
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED 11)

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
    gamewriterfactory.cpp
    ${GraphicsRulesMakerUi_UIS}
    ${GraphicsRulesMakerUi_RCCS}
    ${GraphicsRulesMakerUi_Ico}
    ${PROJECT_BINARY_DIR}/graphicsrulesmakerui.rc
)

# Translations
if(${Qt5LinguistTools_FOUND})
    message(STATUS "Qt Linguist found - will set up translations")
    qt5_create_translation(GraphicsRulesMakerUi_QMS
        ${GraphicsRulesMakerUi_UIS}
        ${GraphicsRulesMakerUi_SRCS}
        "${GraphicsRulesMaker_TRANSLATIONS_DIR}/GraphicsRulesMakerUi_nl.ts"
        OPTIONS -source-language "en-US"
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
    # Install dependencies (Qt) for Windows
    get_target_property(Qt5Core_Location Qt5::Core LOCATION)
    get_filename_component(Qt5Core_BinDir "${Qt5Core_Location}" DIRECTORY)
    get_target_property(Qt5Gui_WindowsPlugin_Location Qt5::QWindowsIntegrationPlugin LOCATION)

    install(FILES ${Qt5Gui_WindowsPlugin_Location}
            DESTINATION bin/platforms
            COMPONENT application
    )

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

        # Fake sysroot to make GetPrerequisites find these as system libraries
        set(gp_sysroot "${PROJECT_BINARY_DIR}/fakewin")
        set(codeInstallPrepare "
            set(ENV{SystemRoot} ${gp_sysroot})

            function(gp_resolve_item_override context item exepath dirs resolved_item_var resolved_var)
                set(override_resolved \${\${resolved_var}})
                # If already resolved, skip this
                if (override_resolved)
                    # Already resolved
                    return()
                endif()

                # Ignore case
                string(TOLOWER \"\${item}\" lower)

                if (lower MATCHES \".*(advapi32|crypt32|dnsapi|gdi32|kernel32|mpr|shell32|user32|ws2_32|d3d9|ole32|msvcrt|mscvrt|winmm).dll\" )
                    set(\${resolved_item_var} \$ENV{SystemRoot}/system/\${lower} PARENT_SCOPE)
                    set(\${resolved_var} 1 PARENT_SCOPE)
                    set(override_resolved 1)
                endif ()

                # Resolve in parent directories, required for plugins with dlls in their base
                set(exe_parent \${exepath})
                while (NOT override_resolved AND NOT exe_parent STREQUAL previous_parent)
                    if (EXISTS \${exe_parent}/\${item})
                      set(\${resolved_item_var} \${exe_parent}/\${item} PARENT_SCOPE)
                      set(\${resolved_var} 1 PARENT_SCOPE)
                      set(override_resolved 1)
                    endif()

                    # Keep previous to detect root (e.g. / on Linux, possibly e.g. C: on Windows)
                    set(previous_parent \${exe_parent})
                    get_filename_component(exe_parent \${exe_parent} DIRECTORY)

                endwhile()

            endfunction()

            function(gp_resolved_file_type_override resolved_item type_var)
                string(TOLOWER \"\${resolved_item}\" lower)

                if (lower MATCHES \"(advapi32|crypt32|dnsapi|gdi32|kernel32|mpr|shell32|user32|ws2_32|d3d9|ole32|oleaut32|msvcrt|mscvrt|winmm).dll\" )
                    set(\${type_var} \"system\" PARENT_SCOPE)
                endif ()

                string(FIND \"\${resolved_item}\" \"\${CMAKE_INSTALL_PREFIX}\" subdirpos)
                if (subdirpos EQUAL 0)
                    set(\${type_var} \"local\" PARENT_SCOPE)
                endif()
            endfunction()
        ")

        # Also install libEGL.dll (indirect dependency of Qt)
        install(FILES ${Qt5Core_BinDir}/libEGL.dll
                DESTINATION bin COMPONENT application)
    endif ()

    # Generate code to fixup bundle during install; installed plugins will be globbed during this step
    install(CODE "
        ${codeInstallPrepare}
        set(gp_tool \"${gp_tool}\")
        file(GLOB_RECURSE BundlePlugins
          \"\${CMAKE_INSTALL_PREFIX}/bin/decoders/*${CMAKE_SHARED_LIBRARY_SUFFIX}\"
          \"\${CMAKE_INSTALL_PREFIX}/bin/views/*${CMAKE_SHARED_LIBRARY_SUFFIX}\"
          \"\${CMAKE_INSTALL_PREFIX}/bin/platform/*${CMAKE_SHARED_LIBRARY_SUFFIX}\"
          \"\${CMAKE_INSTALL_PREFIX}/bin/qml/*${CMAKE_SHARED_LIBRARY_SUFFIX}\"
          \"\${CMAKE_INSTALL_PREFIX}/bin/imageformats/*${CMAKE_SHARED_LIBRARY_SUFFIX}\"
          \"\${CMAKE_INSTALL_PREFIX}/bin/libEGL.dll\"
        )
        include(BundleUtilities)
        fixup_bundle(\"${BundleApp}\" \"\${BundlePlugins}\" \"${BundleLibraryDirs}\")
    " COMPONENT application)
endif(WIN32)
