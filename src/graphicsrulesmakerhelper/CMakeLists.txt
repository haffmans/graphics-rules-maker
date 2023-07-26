cmake_minimum_required(VERSION 3.1.0)
project(GraphicsRulesMakerUi)

# Find other dependencies
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5Network REQUIRED)


include_directories(${GraphicsRulesMaker_INCLUDE_DIRS})

add_executable(GraphicsRulesMakerHelper
    main.cpp
    ${GraphicsRulesMaker_IcoRc}
)
target_link_libraries(GraphicsRulesMakerHelper
    GraphicsRulesMaker
    Qt5::Core
    Qt5::Network
)

install(TARGETS GraphicsRulesMakerHelper
    RUNTIME DESTINATION bin
    COMPONENT application
)
