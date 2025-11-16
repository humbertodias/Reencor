#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <GL/glew.h>
#include <string>
#include <vector>
#include <map>
#include <memory>

// Forward declarations
class Camera;
class Screen;
class InputDevice;
class BaseActiveObject;
class GameScreen;
struct TextureData;

class Game {
public:
    Game();
    ~Game();
    
    bool initialize();
    void run();
    void cleanup();
    
    // Screen management
    void nextScreen(const std::vector<std::string>& screenSequence);
    void screenManager();
    
    // Game loop functions
    void gameplay();
    void display();
    void eventHandler();
    void calculateCameraFocusPoint();
    
    // Resource loading
    void loadDictionaries();
    void inputDeviceAvailable();
    
    // Public members
    std::string type;
    SDL_Window* window;
    SDL_GLContext glContext;
    
    std::pair<int, int> resolution;
    std::pair<int, int> internalResolution;
    int frameRate;
    
    std::unique_ptr<Camera> camera;
    std::unique_ptr<Screen> screen;
    
    std::map<std::string, TextureData> imageDict;
    std::map<std::string, Mix_Chunk*> soundDict;
    std::map<std::string, std::string> objectDict; // JSON strings
    
    std::vector<std::shared_ptr<BaseActiveObject>> objectList;
    std::vector<std::shared_ptr<InputDevice>> inputDeviceList;
    std::shared_ptr<InputDevice> dummyInputDevice;
    
    int emuFrame;
    int hitstop;
    std::vector<float> cameraFocusPoint;
    int superstop;
    std::map<std::string, std::string> cameraPath;
    std::vector<int> frame;
    std::vector<float> pos;
    std::vector<float> drawShake;
    
    bool showBoxes;
    bool showInputs;
    bool active;
    
    int playerNumber;
    std::vector<std::string> selectedCharacters;
    std::string selectedStage;
    
    std::vector<std::string> screenSequence;
    std::shared_ptr<GameScreen> currentScreen;
    
    bool recordInput;
    bool reproduceInput;
    
    std::vector<std::shared_ptr<BaseActiveObject>> activePlayers;
    std::vector<std::shared_ptr<BaseActiveObject>> activeStages;
    
private:
    bool running;
    Uint32 lastFrameTime;
};

#endif // GAME_H
