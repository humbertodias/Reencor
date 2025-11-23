#include "BaseActiveObject.h"
#include "Game.h"
#include "InputDevice.h"
#include <GL/gl.h>

// Movement speed constant
constexpr float MOVEMENT_SPEED = 5.0f;

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
            this->pos[0] -= MOVEMENT_SPEED * face;
        } else if (axis[0] > 0) {
            this->pos[0] += MOVEMENT_SPEED * face;
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
    // Simple placeholder rendering - draw a colored rectangle for each player
    // TODO: Implement proper sprite rendering based on state and animation frame
    
    if (pos.size() < 2) return;
    
    // Calculate screen position (adjust for camera)
    float screenX = pos[0] - (cameraPos.size() > 0 ? cameraPos[0] : 0) + 320; // Center on screen (640/2)
    float screenY = pos[1] - (cameraPos.size() > 1 ? cameraPos[1] : 0) + 200; // Center on screen (400/2)
    
    // Draw a simple colored box to represent the player
    glDisable(GL_TEXTURE_2D);
    
    // Different color for each team
    if (team == 1) {
        glColor4f(0.0f, 0.5f, 1.0f, 1.0f); // Blue for player 1
    } else {
        glColor4f(1.0f, 0.5f, 0.0f, 1.0f); // Orange for player 2
    }
    
    // Draw a rectangle (50x100 pixels)
    float width = 50.0f;
    float height = 100.0f;
    
    glBegin(GL_QUADS);
        glVertex2f(screenX - width/2, screenY - height);
        glVertex2f(screenX + width/2, screenY - height);
        glVertex2f(screenX + width/2, screenY);
        glVertex2f(screenX - width/2, screenY);
    glEnd();
    
    // Draw an outline
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(screenX - width/2, screenY - height);
        glVertex2f(screenX + width/2, screenY - height);
        glVertex2f(screenX + width/2, screenY);
        glVertex2f(screenX - width/2, screenY);
    glEnd();
    
    // Re-enable textures for other rendering
    glEnable(GL_TEXTURE_2D);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void BaseActiveObject::setState(const std::string& stateName) {
    currentState = stateName;
    frame = 0;
    animationFrame = 0;
}
