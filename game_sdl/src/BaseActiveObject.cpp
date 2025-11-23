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
        
        // Check if JSON has a "states" object
        if (jsonData->contains("states") && (*jsonData)["states"].is_object()) {
            auto& statesJson = (*jsonData)["states"];
            
            // Parse all states from JSON
            int stateCount = 0;
            for (auto& [stateName, stateJson] : statesJson.items()) {
                if (stateJson.is_object() && stateJson.contains("framedata") && stateJson["framedata"].is_array()) {
                    StateData state;
                    
                    for (auto& frameJson : stateJson["framedata"]) {
                        FrameData fd;
                        fd.dur = frameJson.value("dur", 1);
                        fd.image = frameJson.value("image", "");
                        
                        // Parse offset if available
                        if (frameJson.contains("pos_offset") && frameJson["pos_offset"].is_array()) {
                            auto offsetArray = frameJson["pos_offset"];
                            if (offsetArray.size() >= 2) {
                                fd.pos_offset = {offsetArray[0], offsetArray[1]};
                            }
                        }
                        
                        state.framedata.push_back(fd);
                    }
                    
                    states[stateName] = state;
                    stateCount++;
                }
            }
            
            std::cout << "Loaded " << stateCount << " states from JSON" << std::endl;
            if (states.find("Stand") != states.end()) {
                std::cout << "  - Stand state has " << states["Stand"].framedata.size() << " frames" << std::endl;
            }
        } else {
            std::cout << "Warning: JSON does not contain 'states' object" << std::endl;
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
        
        // Determine which state we should be in based on input
        std::string targetState = "Stand";
        bool isMoving = false;
        
        // Check for forward/backward movement
        if (axis[0] < 0) {
            isMoving = true;
            // Try various walk state names that might be in the JSON
            if (states.find("Walk Backward") != states.end()) {
                targetState = "Walk Backward";
            } else if (states.find("Walk Back") != states.end()) {
                targetState = "Walk Back";
            } else if (states.find("Walk") != states.end()) {
                targetState = "Walk";
            }
            // Move character
            this->pos[0] -= MOVEMENT_SPEED * face;
        } else if (axis[0] > 0) {
            isMoving = true;
            // Try various walk state names that might be in the JSON
            if (states.find("Walk Forward") != states.end()) {
                targetState = "Walk Forward";
            } else if (states.find("Walk Front") != states.end()) {
                targetState = "Walk Front";
            } else if (states.find("Walk") != states.end()) {
                targetState = "Walk";
            }
            // Move character
            this->pos[0] += MOVEMENT_SPEED * face;
        }
        
        // Check for crouch
        if (axis[1] < 0 && !isMoving) {
            if (states.find("Crouch") != states.end()) {
                targetState = "Crouch";
            }
        }
        
        // Check for jump (axis[1] > 0 means up)
        if (axis[1] > 0 && !isMoving) {
            if (states.find("Neutral Jump") != states.end()) {
                targetState = "Neutral Jump";
            }
        }
        
        // Transition to target state if different from current
        if (targetState != currentState && states.find(targetState) != states.end()) {
            setState(targetState);
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
    float screenY = pos[1] - (cameraPos.size() > 1 ? cameraPos[1] : 0) + 320; // Ground level at Y=320
    
    // Debug: Print state information once per second
    static int debugFrameCounter = 0;
    debugFrameCounter++;
    if (debugFrameCounter % 60 == 0) {
        std::cout << "Player " << team << " - State: " << currentState 
                  << ", Frame: " << animationFrame << "/" << (states.empty() ? 0 : 
                     (states.find(currentState) != states.end() ? states[currentState].framedata.size() : 0))
                  << ", States loaded: " << states.size() << std::endl;
    }
    
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
                    
                    // Debug: Print sprite rendering once per second
                    if (debugFrameCounter % 60 == 0) {
                        std::cout << "  -> Rendering sprite: " << frameData.image 
                                  << " (texture ID: " << textureId << ")" << std::endl;
                    }
                } else {
                    // Debug: Image key not found
                    if (debugFrameCounter % 60 == 0) {
                        std::cout << "  -> Image key not found in imageDict: " << frameData.image << std::endl;
                    }
                }
            } else {
                if (debugFrameCounter % 60 == 0) {
                    std::cout << "  -> Empty image key in frame " << animationFrame << std::endl;
                }
            }
        }
    } else {
        if (debugFrameCounter % 60 == 0) {
            std::cout << "  -> No state data available or state not found: " << currentState << std::endl;
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

// Collision box methods
CollisionBox BaseActiveObject::getHurtbox() const {
    // Default hurtbox for character (approximate body)
    return CollisionBox(pos[0] - 30.0f, pos[1] - 100.0f, 60.0f, 100.0f);
}

CollisionBox BaseActiveObject::getHitbox() const {
    // Default hitbox for attacks (in front of character)
    float hitboxX = (face > 0) ? pos[0] : pos[0] - 50.0f;
    return CollisionBox(hitboxX, pos[1] - 60.0f, 50.0f, 40.0f);
}

CollisionBox BaseActiveObject::getPushbox() const {
    // Pushbox prevents characters from overlapping
    return CollisionBox(pos[0] - 25.0f, pos[1] - 90.0f, 50.0f, 90.0f);
}

void BaseActiveObject::setState(const std::string& stateName) {
    currentState = stateName;
    frame = 0;
    animationFrame = 0;
    frameTimer = 0;
}
