#ifndef INPUT_DEVICE_H
#define INPUT_DEVICE_H

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <map>

class Game;
class BaseActiveObject;

class InputDevice {
public:
    InputDevice(Game* game, int team = 1, int index = 0, const std::string& mode = "none");
    ~InputDevice();
    
    void update();
    void draw(class Screen* screen, const std::vector<float>& cameraPos);
    
    // Input modes
    void keyboardMode();
    void joystickMode();
    void AIMode();
    void recordMode();
    void noneMode();
    void randomMode();
    
    Game* game;
    std::string type;
    int team;
    
    SDL_Joystick* controller;
    std::string controllerName;
    
    std::vector<std::vector<float>> currentInput;
    std::vector<float> rawInput;
    std::vector<float> lastInput;
    std::vector<int> pressCharge;
    std::vector<int> interPress;
    
    std::vector<std::string> pressListShowed;
    BaseActiveObject* activeObject;
    
private:
    void (InputDevice::*modeFunction)();
};

// Keyboard mapping
extern const std::vector<std::vector<int>> keyboardMapping;

// Joystick mapping
extern const std::map<std::string, std::vector<std::vector<std::string>>> joystickNameMapping;

#endif // INPUT_DEVICE_H
