cmake_minimum_required(VERSION 3.16)
project(zelda_like CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2 (adjust if you're doing it differently)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(zelda_like
    src/main.cpp
    src/engine/Engine.cpp
    src/engine/RoomManager.cpp
    src/engine/Camera.cpp
)

target_include_directories(zelda_like
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/engine
)

target_link_libraries(zelda_like
    ${SDL2_LIBRARIES}
)
