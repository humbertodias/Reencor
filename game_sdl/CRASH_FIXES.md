# Runtime Crash Fixes - C++ SDL2 Version

## Summary

This document describes the fixes applied to resolve runtime crashes in the C++ SDL2 version of Reencor.

## Issues Fixed

### 1. SDL_image Not Initialized (CRITICAL)
**Problem**: `IMG_Load()` was being called without initializing SDL_image library.

**Fix**: Added proper initialization in `Game::initialize()`:
```cpp
int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
if (!(IMG_Init(imgFlags) & imgFlags)) {
    std::cerr << "SDL_image initialization failed: " << IMG_GetError() << std::endl;
}
```

Also added cleanup in `Game::shutdown()`:
```cpp
IMG_Quit();
```

### 2. Missing Header Include
**Problem**: Used `IMG_Init()`, `IMG_GetError()`, and `IMG_Quit()` without including the header.

**Fix**: Added to Game.cpp:
```cpp
#include <SDL2/SDL_image.h>
```

### 3. OpenGL Context Attributes Commented Out
**Problem**: OpenGL context attributes were commented out, potentially causing GL errors.

**Fix**: Re-enabled all OpenGL attributes:
```cpp
SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
```

### 4. Division by Zero in Camera Calculation
**Problem**: `calculateCameraFocusPoint()` could divide by zero if all players were invalid.

**Fix**: Count only valid players and check before division:
```cpp
int validPlayers = 0;
for (const auto& player : activePlayers) {
    if (player && player->pos.size() >= 2) {
        sumX += player->pos[0];
        sumY += player->pos[1];
        validPlayers++;
    }
}

if (validPlayers > 0) {
    pos[0] = sumX / validPlayers;
    pos[1] = (sumY / validPlayers) + resolution.second * 0.6f;
    pos[2] = 400.0f;
    cameraFocusPoint = pos;
}
```

### 5. Player Creation Logic
**Problem**: Players were only created if JSON object definitions existed, causing empty game.

**Fix**: Modified `ComboTrialScreen::loadObjects()` to always create players:
```cpp
// Create players even if object definitions don't exist (for basic testing)
for (size_t i = 0; i < game->selectedCharacters.size() && i < 2; i++) {
    // Always create player regardless of JSON data
    auto player = std::make_shared<BaseActiveObject>(...);
    game->activePlayers.push_back(player);
    game->objectList.push_back(player);
}
```

### 6. Null Pointer Safety
**Problem**: Various null pointer dereferences throughout game loop.

**Fix**: Added null checks before dereferencing:
- Camera update: `if (camera) { camera->update(...); }`
- Screen display: `if (screen) { screen->display(); }`
- Input devices: `if (dev) { dev->update(); }`
- Current screen: `if (!currentScreen) { break; }`

### 7. Exception Handling
**Problem**: No exception handling in screen loop.

**Fix**: Added try-catch block:
```cpp
try {
    currentScreen->loop();
}
catch (const std::exception& e) {
    std::cerr << "Exception in screen loop: " << e.what() << std::endl;
    active = false;
    break;
}
```

## Debugging Improvements

Added comprehensive logging throughout:

1. **Initialization status** for each subsystem
2. **Frame counter** with periodic status updates
3. **Object/player counts** printed every 60 frames
4. **Error messages** with context for all failures

Example output:
```
Initializing game...
SDL initialized successfully
SDL_image initialized successfully
Window created: 640x400
OpenGL context created
OpenGL initialized
Input devices: 1
Camera and screen initialized
Loaded 0 images, 0 sounds, 0 objects
Game initialization complete
Screen sequence initialized with ComboTrialScreen
Created player 1 at position -300
Created player 2 at position 300
Starting screen manager with 1 screens
Starting game loop...
Frame 60 - Objects: 2, Players: 2
Frame 120 - Objects: 2, Players: 2
...
```

## Testing Checklist

After these fixes, the application should:
- ✅ Compile without errors
- ✅ Initialize SDL, SDL_image, OpenGL without failures
- ✅ Create and display a window (640x400, black screen)
- ✅ Accept keyboard input (arrow keys, ASDF, QWE)
- ✅ Run at stable 60 FPS
- ✅ Not crash during initialization
- ✅ Not crash during game loop
- ✅ Exit cleanly when closed

## Known Limitations

The following features are not yet implemented (won't cause crashes, just missing functionality):
- Sprite rendering (players are invisible but functional)
- JSON object data parsing
- Sound playback
- Text rendering
- UI elements (menus, HUD)
- Animation system
- Complete collision detection

## Next Steps

To continue development:
1. Implement sprite rendering in `BaseActiveObject::draw()`
2. Add JSON parsing library (e.g., nlohmann/json)
3. Complete collision detection in `CollisionSystem`
4. Add text rendering for debug display
5. Implement remaining game screens

## Commits

- `d487f6c`: Fix crash issues: Initialize SDL_image, add error handling, improve debugging output
- `502c0b3`: Add missing SDL_image.h include and fix division by zero in camera calculation
- `4cd34d6`: Extract debug frame interval to named constant
