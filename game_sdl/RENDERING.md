# Rendering Implementation - C++ SDL2 Version

## Current State

The game now renders basic placeholder graphics to verify the rendering pipeline is working.

## What's Rendered

### Ground Line
- **Color**: Gray (RGB: 0.5, 0.5, 0.5)
- **Position**: Horizontal line at y=200 (center of 400px screen height)
- **Width**: Full screen (0 to 640)
- **Purpose**: Visual reference for ground level

### Player 1 (Left)
- **Color**: Blue (RGB: 0.0, 0.5, 1.0)
- **Initial Position**: x=-300, y=-1 (world coordinates)
- **Size**: 50x100 pixels
- **Outline**: White border
- **Team**: 1
- **Control**: Keyboard (arrow keys for movement)

### Player 2 (Right)
- **Color**: Orange (RGB: 1.0, 0.5, 0.0)
- **Initial Position**: x=300, y=-1 (world coordinates)
- **Size**: 50x100 pixels
- **Outline**: White border
- **Team**: 2
- **Control**: Second input device or dummy input

## Coordinate System

### World to Screen Conversion
```
screenX = worldX - cameraX + 320  // 320 = 640/2 (screen center)
screenY = worldY - cameraY + 200  // 200 = 400/2 (screen center)
```

### Player Positioning
- Players at y=-1 means they're 1 pixel above ground (y=0)
- With camera at y=0, they render at screen y=200 (on the ground line)
- Players separated by 600 world units (-300 to +300)

## Rendering Pipeline

1. **Screen::display()** - Clear screen and reset matrix
2. **Game::display()** - Draw ground line, then all objects
3. **BaseActiveObject::draw()** - Draw player rectangles
4. **SDL_GL_SwapWindow()** - Display the frame

## Movement

Players can move using keyboard input:
- **Left Arrow**: Move left
- **Right Arrow**: Move right
- **Up Arrow**: Jump (changes state to "Jump")
- **Down Arrow**: Crouch (changes state to "Crouch")

Movement speed: 5.0 pixels per frame

## Next Steps: Sprite Rendering

To implement proper sprite rendering, the following needs to be done:

1. **Sprite Data Structure**
   - Parse JSON files to get sprite definitions
   - Map state names to sprite texture IDs
   - Store animation frame sequences

2. **Animation System**
   - Track current animation frame
   - Update frame based on game time
   - Handle frame looping and transitions

3. **Sprite Drawing**
   - Look up texture ID based on current state
   - Calculate sprite position and scale
   - Use `Renderer::drawSprite()` to render texture
   - Apply facing direction (flip sprite for left/right)

4. **Hitbox Rendering (Debug)**
   - Draw collision boxes when `showBoxes` is enabled
   - Use different colors for hurtbox, hitbox, pushbox

## Code Locations

- **Player rendering**: `game_sdl/src/BaseActiveObject.cpp` (line 57-100)
- **Ground line**: `game_sdl/src/Game.cpp` (line 282-291)
- **Sprite drawing utility**: `game_sdl/src/Renderer.cpp` (line 98-107)

## Assets Ready

The game successfully loaded:
- **2632 textures** (character sprites, effects, UI elements)
- All converted to RGBA32 format for OpenGL
- Stored in `Game::imageDict` map with string keys

Example key format: `"11_Ken/14701"` for Ken's sprites

## Testing

Run the game and verify:
- ✅ Window opens at 640x400
- ✅ Ground line visible (gray horizontal line)
- ✅ Two colored rectangles (blue and orange)
- ✅ Rectangles move with arrow keys
- ✅ Frame counter updates in console every 60 frames
