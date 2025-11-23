#include "BaseActiveObject.h"
#include "Game.h"
#include "InputDevice.h"

BaseActiveObject::BaseActiveObject(Game* game, 
                                  const std::unordered_map<std::string, void*>& dict,
                                  const std::vector<float>& pos,
                                  int face,
                                  std::shared_ptr<InputDevice> inputDevice,
                                  int team)
    : GameObject(game),
      dict(dict),
      face(face),
      team(team),
      inputDevice(inputDevice),
      currentState("Stand"),
      frame(0),
      animationFrame(0)
{
    this->pos = pos;
    this->type = "active_object";
}

void BaseActiveObject::update(const std::vector<float>& cameraFocusPoint) {
    // Update animation frame
    frame++;
    
    // Handle input if device is available
    if (inputDevice) {
        auto axis = inputDevice->getAxis();
        
        // Basic movement logic
        if (axis[0] < 0) {
            this->pos[0] -= 5.0f * face;
        } else if (axis[0] > 0) {
            this->pos[0] += 5.0f * face;
        }
        
        if (axis[1] < 0) {
            // Jump
            currentState = "Jump";
        } else if (axis[1] > 0) {
            // Crouch
            currentState = "Crouch";
        } else {
            currentState = "Stand";
        }
    }
    
    animationFrame++;
}

void BaseActiveObject::draw(void* screen, const std::vector<float>& cameraPos) {
    // TODO: Implement sprite rendering
    // This would need to load and render sprites based on current state and animation frame
}

void BaseActiveObject::setState(const std::string& stateName) {
    currentState = stateName;
    frame = 0;
    animationFrame = 0;
}
