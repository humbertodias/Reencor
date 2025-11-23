cmake_minimum_required(VERSION 3.15)
project(Reencor VERSION 0.3.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(OpenGL REQUIRED)

# Include directories
include_directories(
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_MIXER_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

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
)

# Create executable
add_executable(reencor ${SOURCES})

# Link libraries
target_link_libraries(reencor
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
    ${OPENGL_LIBRARIES}
    m
)

# Copy assets to build directory
add_custom_command(TARGET reencor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_SOURCE_DIR}/../Assets
    ${CMAKE_BINARY_DIR}/Assets
)
