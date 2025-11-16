cmake_minimum_required(VERSION 3.10)
project(Reencor VERSION 0.3.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# Source files
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/OpenGLRenderer.cpp
    src/InputDevice.cpp
    src/BaseActiveObject.cpp
    src/GameScreens.cpp
    src/CollisionDetection.cpp
    src/CommonFunctions.cpp
    src/InterfaceObjects.cpp
)

# Header files
set(HEADERS
    src/Game.h
    src/OpenGLRenderer.h
    src/InputDevice.h
    src/BaseActiveObject.h
    src/GameScreens.h
    src/CollisionDetection.h
    src/CommonFunctions.h
    src/InterfaceObjects.h
)

# Create executable
add_executable(Reencor ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(Reencor PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(Reencor
    ${SDL2_LIBRARIES}
    SDL2_mixer::SDL2_mixer
    SDL2_image::SDL2_image
    ${OPENGL_LIBRARIES}
    GLEW::GLEW
)

# Platform-specific settings
if(WIN32)
    target_link_libraries(Reencor opengl32)
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    target_link_libraries(Reencor ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY})
endif()

# Copy assets to build directory
add_custom_command(TARGET Reencor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Assets $<TARGET_FILE_DIR:Reencor>/Assets
)

# Copy font to build directory
add_custom_command(TARGET Reencor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/Util/unispace\ bd.ttf $<TARGET_FILE_DIR:Reencor>/unispace_bd.ttf
)

# Installation
install(TARGETS Reencor DESTINATION bin)
install(DIRECTORY Assets DESTINATION share/reencor)
install(FILES Util/unispace\ bd.ttf DESTINATION share/reencor)
