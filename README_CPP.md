# Reencor C++ Port

This is a C++ port of the Reencor fighting game using SDL2 and OpenGL.

## Building

### Requirements

- CMake 3.10+
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- SDL2
- SDL2_mixer
- SDL2_image
- OpenGL
- GLEW

### Ubuntu/Debian

```bash
sudo apt-get update
sudo apt-get install cmake build-essential libsdl2-dev libsdl2-mixer-dev libsdl2-image-dev libglew-dev libglu1-mesa-dev
```

### macOS

```bash
brew install cmake sdl2 sdl2_mixer sdl2_image glew
```

### Build Instructions

#### Using the build script (recommended):
```bash
chmod +x build.sh
./build.sh
```

#### Manual build:
```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

#### Run the game:
```bash
./build/Reencor
```

**Note**: Make sure the `Assets` folder with game sprites and data is present in the repository root.

## Architecture

The C++ port maintains a similar architecture to the original Python version:

- **Game.cpp/h**: Main game class managing the game loop, resources, and state
- **OpenGLRenderer.cpp/h**: OpenGL rendering system with texture loading and drawing
- **InputDevice.cpp/h**: Keyboard and joystick input handling
- **BaseActiveObject.cpp/h**: Base class for all game objects (characters, stages, etc.)
- **GameScreens.cpp/h**: Different game screens (title, mode selection, gameplay)
- **CollisionDetection.cpp/h**: Collision detection system
- **CommonFunctions.cpp/h**: Utility functions
- **InterfaceObjects.cpp/h**: UI elements and messages

## Features Implemented

- [x] Basic game framework with SDL2
- [x] OpenGL rendering setup
- [x] Texture loading from PNG/JPG
- [x] Input handling (keyboard and joystick)
- [x] Audio system with SDL_mixer
- [x] Camera system
- [x] Basic game object system
- [x] Screen management
- [ ] Full character system (in progress)
- [ ] Full collision detection (in progress)
- [ ] Game screens and menus (in progress)
- [ ] JSON data loading (in progress)

## Note

This is an initial port providing the core framework. Many game-specific features from the original Python version are still being implemented. The game loop, rendering, and input systems are functional and provide a foundation for completing the port.

## Comparison with Python Version

See [PYTHON_VS_CPP.md](PYTHON_VS_CPP.md) for a detailed comparison between the Python/Pygame and C++/SDL2 versions.

## Controls

Same as the original Python version - see main README.md for control details.

## Contributing

To add features or complete the port:
1. Review the Python implementation in the corresponding module
2. Port the logic to the C++ equivalent
3. Test thoroughly
4. Submit a pull request

## License

Same as the original Reencor project.
