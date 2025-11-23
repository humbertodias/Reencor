#ifndef BASEACTIVEOBJECT_H
#define BASEACTIVEOBJECT_H

#include "GameObject.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

class InputDevice;

// Simple structure to hold frame data
struct FrameData {
    int dur = 1;  // Duration in frames
    std::string image;  // Image key (e.g., "02_Ryu/02593")
};

// Simple structure to hold state data
struct StateData {
    std::vector<FrameData> framedata;
};

class BaseActiveObject : public GameObject {
public:
    BaseActiveObject(Game* game, 
                    const std::unordered_map<std::string, void*>& dict,
                    const std::vector<float>& pos = {0, 0, 0},
                    int face = 1,
                    std::shared_ptr<InputDevice> inputDevice = nullptr,
                    int team = 0);
    
    ~BaseActiveObject() override = default;

    void update(const std::vector<float>& cameraFocusPoint) override;
    void draw(void* screen, const std::vector<float>& cameraPos) override;

    // State management
    void setState(const std::string& stateName);
    std::string getState() const { return currentState; }
    
    // Animation data
    std::unordered_map<std::string, StateData> states;

    int face;
    int team;
    std::string currentState;
    std::shared_ptr<InputDevice> inputDevice;

private:
    std::unordered_map<std::string, void*> dict;
    int frame;  // Current frame counter within the state
    int animationFrame;  // Current frame index in the framedata array
    int frameTimer;  // Timer for current frame duration
};

#endif // BASEACTIVEOBJECT_H
