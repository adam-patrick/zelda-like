cmake_minimum_required(VERSION 3.16)

project(zelda_like
    VERSION 0.1.0
    DESCRIPTION "Top-down Zelda-like prototype in C++ and SDL2"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_WINDOWS "Build for Windows" OFF)

find_package(SDL2 REQUIRED)

set(SOURCES
    src/main.cpp
    src/engine/Engine.cpp
    src/game/Player.cpp
    src/game/TileMap.cpp
    src/game/Enemy.cpp       # <--
)

set(HEADERS
    src/engine/Engine.h
    src/game/Player.h
    src/game/TileMap.h
    src/game/Enemy.h         # <--
)

add_executable(zelda_like ${SOURCES} ${HEADERS})

target_include_directories(zelda_like
    PRIVATE
        ${SDL2_INCLUDE_DIRS}
        src
)

target_link_libraries(zelda_like
    PRIVATE
        ${SDL2_LIBRARIES}
)
