#ifndef BASEACTIVEOBJECT_H
#define BASEACTIVEOBJECT_H

#include "GameObject.h"
#include <string>
#include <memory>
#include <unordered_map>

class InputDevice;

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

    int face;
    int team;
    std::string currentState;
    std::shared_ptr<InputDevice> inputDevice;

private:
    std::unordered_map<std::string, void*> dict;
    int frame;
    int animationFrame;
};

#endif // BASEACTIVEOBJECT_H
