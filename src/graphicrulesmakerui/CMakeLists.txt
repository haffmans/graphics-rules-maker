cmake_minimum_required(VERSION 3.0.0)
project(GraphicRulesMakerUi)

# We need C++11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# Find other dependencies
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED)
if (NOT GraphicRulesMaker_FULL_BUILD)
    find_package(GraphicRulesMaker REQUIRED)
endif (NOT GraphicRulesMaker_FULL_BUILD)
find_package(ImageMagick REQUIRED COMPONENTS convert)

include_directories(${GraphicRulesMaker_INCLUDE_DIRS})

qt5_wrap_ui(GraphicRulesMaker_UIS
    mainwindow.ui
    manualsaveconfirmationbox.ui
    aboutdialog.ui
)

qt5_add_resources(GraphicRulesMakerUi_RCCS
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
    COMMENT "Resizing icon"
    MAIN_DEPENDENCY "${PROJECT_SOURCE_DIR}/icon.png"
    DEPENDS "icon-48.png" "icon-32.png" "icon-16.png"
)

set(GraphicRulesMakerUi_Ico "${PROJECT_BINARY_DIR}/icon.ico")

configure_file(graphicrulesmakerui.rc.in
    ${PROJECT_BINARY_DIR}/graphicrulesmakerui.rc)

# GraphicRulesMaker UI
set(GraphicRulesMakerUi_SRCS
    main.cpp
    mainwindow.cpp
    manualsaveconfirmationbox.cpp
    aboutdialog.cpp
    gamewriterfactory.cpp
    ${GraphicRulesMaker_UIS}
    ${GraphicRulesMakerUi_RCCS}
    ${GraphicRulesMakerUi_Ico}
    ${PROJECT_BINARY_DIR}/graphicrulesmakerui.rc
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
