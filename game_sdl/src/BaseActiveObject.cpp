#include "BaseActiveObject.h"
#include "Game.h"
#include "InputDevice.h"
#include "Renderer.h"
#include <GL/gl.h>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

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
      animationFrame(0),
      frameTimer(0)
{
    this->pos = pos;
    this->type = "active_object";
    
    // Load JSON data if available in dict
    if (dict.find("json") != dict.end() && dict.at("json") != nullptr) {
        json* jsonData = static_cast<json*>(dict.at("json"));
        
        // Parse Stand state for now (minimal implementation)
        if (jsonData->contains("Stand")) {
            StateData standState;
            auto& standJson = (*jsonData)["Stand"];
            
            if (standJson.contains("framedata") && standJson["framedata"].is_array()) {
                for (auto& frameJson : standJson["framedata"]) {
                    FrameData fd;
                    fd.dur = frameJson.value("dur", 1);
                    fd.image = frameJson.value("image", "");
                    standState.framedata.push_back(fd);
                }
            }
            
            states["Stand"] = standState;
            std::cout << "Loaded Stand state with " << standState.framedata.size() << " frames" << std::endl;
        }
    }
}

void BaseActiveObject::update(const std::vector<float>& cameraFocusPoint) {
    // Update frame timer
    frameTimer++;
    
    // Check if we need to advance to next animation frame
    if (!states.empty() && states.find(currentState) != states.end()) {
        auto& state = states[currentState];
        if (!state.framedata.empty()) {
            auto& currentFrame = state.framedata[animationFrame];
            if (frameTimer >= currentFrame.dur) {
                frameTimer = 0;
                animationFrame = (animationFrame + 1) % state.framedata.size();
            }
        }
    }
    
    // Handle input if device is available
    if (inputDevice) {
        auto axis = inputDevice->getAxis();
        
        // Basic movement logic
        if (axis[0] < 0) {
            this->pos[0] -= MOVEMENT_SPEED * face;
        } else if (axis[0] > 0) {
            this->pos[0] += MOVEMENT_SPEED * face;
        }
    }
    
    frame++;
}

void BaseActiveObject::draw(void* screen, const std::vector<float>& cameraPos) {
    if (pos.size() < 2) {
        std::cerr << "Warning: pos vector too small in BaseActiveObject::draw()" << std::endl;
        return;
    }
    
    // Calculate screen position (adjust for camera)
    float screenX = pos[0] - (cameraPos.size() > 0 ? cameraPos[0] : 0) + 320; // Center on screen (640/2)
    float screenY = pos[1] - (cameraPos.size() > 1 ? cameraPos[1] : 0) + 200; // Center on screen (400/2)
    
    // Try to render sprite if we have state data
    bool renderedSprite = false;
    if (!states.empty() && states.find(currentState) != states.end()) {
        auto& state = states[currentState];
        if (!state.framedata.empty() && animationFrame < state.framedata.size()) {
            auto& frameData = state.framedata[animationFrame];
            if (!frameData.image.empty()) {
                // Look up the texture from game's imageDict
                auto& imageDict = game->imageDict;
                if (imageDict.find(frameData.image) != imageDict.end()) {
                    GLuint textureId = imageDict[frameData.image];
                    
                    // Default sprite size (will be overridden by actual texture size)
                    float spriteWidth = 100.0f;
                    float spriteHeight = 150.0f;
                    
                    // Adjust for facing direction
                    bool flipX = (face < 0);
                    
                    // Render the sprite
                    Renderer::drawSprite(textureId, screenX, screenY, spriteWidth, spriteHeight, flipX);
                    renderedSprite = true;
                }
            }
        }
    }
    
    // Fallback: Draw placeholder rectangle if no sprite rendered
    if (!renderedSprite) {
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
}

void BaseActiveObject::setState(const std::string& stateName) {
    currentState = stateName;
    frame = 0;
    animationFrame = 0;
    frameTimer = 0;
}
