#ifndef BASE_ACTIVE_OBJECT_H
#define BASE_ACTIVE_OBJECT_H

#include <vector>
#include <string>
#include <map>
#include <memory>

class Game;
class InputDevice;
class Screen;

class BaseActiveObject {
public:
    BaseActiveObject(
        Game* game,
        const std::string& type = "character",
        const std::map<std::string, std::string>& dict = {},
        int team = 1,
        InputDevice* inputDevice = nullptr,
        const std::vector<float>& pos = {280, 200, 0},
        int face = 1,
        int palette = 0,
        const std::string& initialState = ""
    );
    
    virtual ~BaseActiveObject() = default;
    
    virtual void update(const std::vector<float>& cameraFocusPoint);
    virtual void draw(Screen* screen, const std::vector<float>& cameraPos);
    
    Game* game;
    std::string type;
    std::map<std::string, std::string> dict;
    int team;
    InputDevice* inputDevice;
    std::vector<float> pos;
    int face;
    int palette;
    
    // Collision data
    std::vector<std::string> hurtCollHit;
    std::vector<std::string> hitCollHurt;
    std::vector<std::string> triggerCollHurt;
    std::vector<std::string> takeCollGrab;
    
    // Visual properties
    std::string image;
    std::vector<float> imageOffset;
    std::vector<float> imageSize;
    std::vector<bool> imageMirror;
    std::vector<int> imageTint;
    std::vector<float> imageAngle;
    bool imageRepeat;
    float imageGlow;
    
    // State
    std::string currentState;
    int stateFrame;
};

#endif // BASE_ACTIVE_OBJECT_H
