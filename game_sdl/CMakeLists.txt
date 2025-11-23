cmake_minimum_required(VERSION 3.15)
project(reencor VERSION 0.3.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)
pkg_check_modules(SDL2_GFX REQUIRED SDL2_gfx)

# Try to find nlohmann_json
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    # If not found as package, check if it's installed system-wide
    find_path(NLOHMANN_JSON_INCLUDE_DIR nlohmann/json.hpp
        PATHS /usr/include /usr/local/include)
    if(NLOHMANN_JSON_INCLUDE_DIR)
        set(nlohmann_json_FOUND TRUE)
        message(STATUS "Found nlohmann_json include dir: ${NLOHMANN_JSON_INCLUDE_DIR}")
    endif()
endif()

# Include directories
include_directories(
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_MIXER_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(nlohmann_json_FOUND AND NLOHMANN_JSON_INCLUDE_DIR)
    include_directories(${NLOHMANN_JSON_INCLUDE_DIR})
endif()

# Print include directories
message(STATUS "SDL2 include dirs: ${SDL2_INCLUDE_DIRS}")
message(STATUS "SDL2_image include dirs: ${SDL2_IMAGE_INCLUDE_DIRS}")
message(STATUS "SDL2_mixer include dirs: ${SDL2_MIXER_INCLUDE_DIRS}")
message(STATUS "OpenGL include dirs: ${OPENGL_INCLUDE_DIRS}")

# Print libraries
message(STATUS "SDL2 libraries: ${SDL2_LIBRARIES}")
message(STATUS "SDL2_image libraries: ${SDL2_IMAGE_LIBRARIES}")
message(STATUS "SDL2_mixer libraries: ${SDL2_MIXER_LIBRARIES}")
message(STATUS "OpenGL libraries: ${OPENGL_LIBRARIES}")

# Source files
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/GameObject.cpp
    src/Camera.cpp
    src/Screen.cpp
    src/InputDevice.cpp
    src/BaseActiveObject.cpp
    src/AssetLoader.cpp
    src/GameScreen.cpp
    src/Renderer.cpp
    src/CollisionSystem.cpp
    src/HUD.cpp
    src/ComboTrial.cpp
)

# Create executable
add_executable(reencor ${SOURCES})

target_link_libraries(reencor
        -L${SDL2_LIBRARY_DIRS}
        -L${SDL2_IMAGE_LIBRARY_DIRS}
        -L${SDL2_GFX_LIBRARY_DIRS}
        -lSDL2
        -lSDL2_image
        -lSDL2_mixer
        -lSDL2_gfx
        -lGL
        m
)

# Copy assets to build directory
add_custom_command(TARGET reencor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_SOURCE_DIR}/../Assets
    ${CMAKE_BINARY_DIR}/Assets
)
