cmake_minimum_required(VERSION 3.16)
project(zelda_like CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- SDL2 core ---
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# --- SDL2_image (manual style) ---
# We’ll try to find SDL2_image ourselves.
find_path(SDL2_IMAGE_INCLUDE_DIR SDL_image.h
    /usr/include
    /usr/include/SDL2
    /usr/local/include
    /usr/local/include/SDL2
)

find_library(SDL2_IMAGE_LIBRARY
    NAMES SDL2_image
    PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu
)

if (NOT SDL2_IMAGE_INCLUDE_DIR OR NOT SDL2_IMAGE_LIBRARY)
    message(FATAL_ERROR "SDL2_image not found. Did you install libsdl2-image-dev?")
endif()

include_directories(${SDL2_IMAGE_INCLUDE_DIR})

add_executable(zelda_like
    src/main.cpp
    src/engine/Engine.cpp
    src/engine/Camera.cpp
    src/engine/RoomManager.cpp
    src/engine/TextureManager.cpp
)

target_include_directories(zelda_like
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/engine
)

target_link_libraries(zelda_like
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARY}
)

# After building, copy the assets/ folder next to the binary so
# runtime paths like "assets/tiles.png" will resolve.
add_custom_command(
    TARGET zelda_like
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            ${CMAKE_BINARY_DIR}/assets
)
