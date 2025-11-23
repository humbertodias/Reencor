#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

class GameObject;
class Camera;
class Screen;
class InputDevice;
class GameScreen;
class BaseActiveObject;

class Game {
public:
    Game();
    ~Game();

    bool initialize();
    void run();
    void shutdown();

    // Screen management
    void nextScreen(const std::vector<std::string>& screenSequence);
    void screenManager();

    // Game loop functions
    void eventHandler();
    void gameplay();
    void display();
    void calculateCameraFocusPoint();

    // Getters
    SDL_Window* getWindow() const { return window; }
    int getFrameRate() const { return frameRate; }
    std::pair<int, int> getResolution() const { return resolution; }
    std::pair<int, int> getInternalResolution() const { return internalResolution; }

    // Public members (matching Python version)
    std::string type;
    std::pair<int, int> resolution;
    std::pair<int, int> internalResolution;
    int frameRate;
    int emuFrame;
    int hitstop;
    int superstop;
    bool showBoxes;
    bool showInputs;
    bool active;
    int playerNumber;

    std::vector<float> cameraFocusPoint;
    std::vector<float> pos;
    std::vector<std::string> selectedCharacters;
    std::string selectedStage;

    // Asset dictionaries
    std::unordered_map<std::string, GLuint> imageDict;
    std::unordered_map<std::string, Mix_Chunk*> soundDict;
    std::unordered_map<std::string, void*> objectDict;

    // Object lists
    std::vector<std::shared_ptr<GameObject>> objectList;
    std::vector<std::shared_ptr<BaseActiveObject>> activePlayers;
    std::shared_ptr<BaseActiveObject> activeStages;

    // Input devices
    std::vector<std::shared_ptr<InputDevice>> inputDeviceList;

    // Core systems
    std::shared_ptr<Camera> camera;
    std::shared_ptr<Screen> screen;

private:
    SDL_Window* window;
    SDL_GLContext glContext;
    std::vector<std::shared_ptr<GameScreen>> screenSequence;
    std::shared_ptr<GameScreen> currentScreen;

    void initAudio();
    void loadAssets();
    void inputDeviceAvailable();
};

#endif // GAME_H
