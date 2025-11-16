# Contributing to the C++ Port

Thank you for your interest in contributing to the Reencor C++ port! This guide will help you understand how to contribute effectively.

## Getting Started

1. **Familiarize yourself with both versions**:
   - Study the Python version in `main.py` and `Util/` folder
   - Review the C++ implementation in `src/` folder
   - Read [PYTHON_VS_CPP.md](PYTHON_VS_CPP.md) for architecture comparison

2. **Set up your development environment**:
   ```bash
   # Install dependencies (Ubuntu/Debian)
   sudo apt-get install cmake build-essential libsdl2-dev libsdl2-mixer-dev \
                        libsdl2-image-dev libglew-dev libglu1-mesa-dev
   
   # Build the project
   ./build.sh
   ```

## What Needs to Be Done

Check the current status in [README_CPP.md](README_CPP.md). Priority areas include:

1. **Character System** - Port from `Util/Active_Objects.py`
   - Complete state machine implementation
   - Animation system
   - Move execution
   - Combo system

2. **Collision Detection** - Port from `Util/Box_Collitions.py`
   - Hit/hurt box calculations
   - Collision response
   - Grab and trigger systems

3. **Game Screens** - Port from `Util/Game_Screens.py`
   - Title screen
   - Character select
   - Training mode
   - Combo trials

4. **JSON Data Loading**
   - Add JSON library (nlohmann/json recommended)
   - Parse character data
   - Parse stage data
   - Parse move definitions

5. **UI System** - Port from `Util/Interface_objects.py`
   - Health bars
   - Combo counters
   - Menu system
   - Messages

## Code Style Guidelines

### C++ Best Practices

1. **Use modern C++17 features**:
   ```cpp
   // Good
   auto texture = std::make_unique<Texture>();
   
   // Avoid
   Texture* texture = new Texture();
   ```

2. **Follow RAII principles**:
   ```cpp
   // Resources automatically cleaned up
   class Texture {
   public:
       Texture(const std::string& path);
       ~Texture(); // Frees OpenGL texture
   private:
       GLuint id;
   };
   ```

3. **Use const correctness**:
   ```cpp
   void draw(const std::vector<float>& pos) const;
   ```

4. **Prefer references over pointers when possible**:
   ```cpp
   void update(Game& game);  // Better than Game*
   ```

### Naming Conventions

- **Classes**: PascalCase (e.g., `BaseActiveObject`)
- **Functions**: camelCase (e.g., `updateGameState`)
- **Variables**: camelCase (e.g., `frameRate`)
- **Constants**: UPPER_CASE (e.g., `MAX_PLAYERS`)
- **Member variables**: camelCase (e.g., `currentState`)

### File Organization

- Header files: `.h` extension
- Implementation files: `.cpp` extension
- One class per file pair (exceptions for small helper classes)
- Include guards in all headers

## Porting Process

### Step-by-Step Guide

1. **Choose a module to port** (e.g., collision detection)

2. **Understand the Python implementation**:
   ```python
   # Python version
   def calculate_boxes_collisions(game):
       for obj1 in game.object_list:
           for obj2 in game.object_list:
               if box_collide(obj1, obj2):
                   handle_collision(obj1, obj2)
   ```

3. **Create equivalent C++ structure**:
   ```cpp
   // C++ version
   void calculateBoxesCollisions(Game* game) {
       for (auto& obj1 : game->objectList) {
           for (auto& obj2 : game->objectList) {
               if (boxCollide(obj1.get(), obj2.get())) {
                   handleCollision(obj1.get(), obj2.get());
               }
           }
       }
   }
   ```

4. **Test incrementally**:
   - Add debug output
   - Verify behavior matches Python version
   - Remove debug code

5. **Update documentation**:
   - Add comments for complex logic
   - Update README_CPP.md if adding major features
   - Mark feature as complete in tracking documents

## Testing

### Building and Testing

```bash
# Clean build
rm -rf build
./build.sh

# Run with verbose output
./build/Reencor --verbose

# Check for memory leaks (optional, with valgrind)
valgrind --leak-check=full ./build/Reencor
```

### Verification

Compare behavior with Python version:
```bash
# Run Python version
python main.py

# Run C++ version
./build/Reencor

# Verify same behavior
```

## Common Pitfalls

1. **Python lists → C++ vectors**:
   ```python
   # Python
   pos = [x, y, z]
   ```
   ```cpp
   // C++
   std::vector<float> pos = {x, y, z};
   ```

2. **Python dictionaries → C++ maps**:
   ```python
   # Python
   image_dict = {"key": value}
   ```
   ```cpp
   // C++
   std::map<std::string, TextureData> imageDict;
   imageDict["key"] = value;
   ```

3. **Memory management**:
   - Use smart pointers (`unique_ptr`, `shared_ptr`)
   - Avoid raw `new`/`delete`

4. **String handling**:
   ```cpp
   // Use std::string, not char*
   std::string name = "Ryu";
   ```

## Submitting Changes

1. **Fork the repository**

2. **Create a feature branch**:
   ```bash
   git checkout -b feature/collision-detection
   ```

3. **Make your changes**:
   - Follow code style guidelines
   - Add comments for complex logic
   - Test thoroughly

4. **Commit with clear messages**:
   ```bash
   git commit -m "Implement collision detection system"
   ```

5. **Push and create pull request**:
   ```bash
   git push origin feature/collision-detection
   ```

6. **In the PR description**:
   - Explain what was ported
   - Reference the Python module
   - Describe any differences or improvements
   - Include test results

## Resources

- [SDL2 Documentation](https://wiki.libsdl.org/)
- [OpenGL Reference](https://www.khronos.org/opengl/wiki/)
- [C++17 Features](https://en.cppreference.com/w/cpp/17)
- [nlohmann/json Library](https://github.com/nlohmann/json)

## Questions?

If you have questions:
1. Check existing code for examples
2. Review the Python implementation
3. Open an issue for discussion
4. Join the community chat (if available)

## Code of Conduct

- Be respectful and constructive
- Test your changes before submitting
- Document your code
- Help others learn from your contributions

Thank you for contributing to Reencor!
