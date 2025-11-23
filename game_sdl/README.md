# Reencor - SDL2 C++ Version

This is a C++ SDL2 port of the Reencor fighting game, originally written in Python with Pygame.

## Overview

This version maintains the core gameplay mechanics of the original Pygame version while leveraging SDL2 and OpenGL for rendering and input handling in C++.

## Features

- SDL2-based window management and input handling
- OpenGL rendering for 2D graphics
- SDL2_mixer for audio playback
- Keyboard and joystick support
- Modular architecture with separate systems for:
  - Game loop management
  - Input handling
  - Rendering
  - Collision detection
  - Asset loading

## Requirements

### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    libsdl2-dev \
    libsdl2-image-dev \
    libsdl2-mixer-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev
```

### macOS
```bash
brew install cmake sdl2 sdl2_image sdl2_mixer
```

### Windows
Download and install:
- CMake: https://cmake.org/download/
- SDL2 development libraries: https://www.libsdl.org/download-2.0.php
- SDL2_image: https://www.libsdl.org/projects/SDL_image/
- SDL2_mixer: https://www.libsdl.org/projects/SDL_mixer/

## Building

1. Navigate to the game_sdl directory:
```bash
cd game_sdl
```

2. Create a build directory:
```bash
mkdir build
cd build
```

3. Generate build files with CMake:
```bash
cmake ..
```

4. Build the project:
```bash
make
```

5. Run the game:
```bash
./reencor
```

## Project Structure

```
game_sdl/
├── CMakeLists.txt          # Build configuration
├── include/                # Header files
│   ├── Game.h             # Main game class
│   ├── GameObject.h       # Base game object
│   ├── Camera.h           # Camera system
│   ├── Screen.h           # Screen/display management
│   ├── InputDevice.h      # Input handling
│   ├── BaseActiveObject.h # Active game objects (players, etc.)
│   ├── GameScreen.h       # Game screens/modes
│   ├── AssetLoader.h      # Asset loading system
│   ├── Renderer.h         # OpenGL rendering utilities
│   └── CollisionSystem.h  # Collision detection
├── src/                    # Implementation files
│   ├── main.cpp
│   ├── Game.cpp
│   ├── GameObject.cpp
│   ├── Camera.cpp
│   ├── Screen.cpp
│   ├── InputDevice.cpp
│   ├── BaseActiveObject.cpp
│   ├── GameScreen.cpp
│   ├── AssetLoader.cpp
│   ├── Renderer.cpp
│   └── CollisionSystem.cpp
└── README.md              # This file
```

## Architecture

### Core Components

1. **Game**: Main game loop and state management
2. **GameObject**: Base class for all game entities
3. **Camera**: Handles camera movement and focus
4. **Screen**: Manages display and rendering
5. **InputDevice**: Handles keyboard and joystick input
6. **BaseActiveObject**: Represents active entities (players, stages)
7. **GameScreen**: Different game modes (title, combo trial, etc.)
8. **AssetLoader**: Loads images, sounds, and JSON data
9. **Renderer**: OpenGL rendering utilities
10. **CollisionSystem**: Handles collision detection and response

### Key Differences from Pygame Version

- **Language**: C++ instead of Python
- **Rendering**: Direct OpenGL calls instead of Pygame's surface-based rendering
- **Memory Management**: Smart pointers for automatic memory management
- **Performance**: Compiled code offers better performance
- **Type Safety**: Static typing catches errors at compile time

## Controls

Same as the Pygame version:

| Action              | Input                    |
|---------------------|--------------------------|
| Move Left           | ←                        |
| Move Right          | →                        |
| Crouch              | ↓                        |
| Jump                | ↑                        |
| Light Punch (LP)    | A                        |
| Medium Punch (MP)   | S                        |
| Heavy Punch (HP)    | D                        |
| Light Kick (LK)     | Q                        |
| Medium Kick (MK)    | W                        |
| Heavy Kick (HK)     | E                        |

## Current Status

This is a work-in-progress port. Currently implemented:
- ✅ Basic game loop
- ✅ Window and OpenGL initialization
- ✅ Input handling (keyboard and joystick)
- ✅ Camera system
- ✅ Basic object management
- ✅ Asset loading framework

Still in development:
- ⏳ Complete sprite rendering
- ⏳ Animation system
- ⏳ Full collision detection
- ⏳ JSON parsing for game data
- ⏳ Sound system integration
- ⏳ UI elements (menus, HUD)
- ⏳ All game modes

## Development Notes

- The code structure mirrors the original Python version where possible
- Uses C++17 features for modern C++ development
- Smart pointers are used for automatic memory management
- The build system uses CMake for cross-platform compatibility

## Contributing

When contributing to this SDL2 version:
1. Follow the existing code structure
2. Maintain compatibility with the original Pygame version's gameplay
3. Write clear comments for complex logic
4. Test on multiple platforms if possible

## License

Same as the main Reencor project. See LICENSE in the root directory.

## Assets

This version uses the same assets as the Pygame version. Make sure the Assets folder is accessible from the build directory (the CMake configuration creates a symlink automatically).
