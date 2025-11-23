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
        
        // Load collision boxes from JSON
        loadBoxesFromJSON();
        
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
        // Input axis mapping: axis[1] > 0 = DOWN, axis[1] < 0 = UP
        bool isCrouching = (axis[1] > 0);  // DOWN
        bool isJumping = (axis[1] < 0);     // UP
        
        // Priority: Attacks > Jump > Movement > Crouch > Stand
        
        // Check for attack buttons (punches)
        if (inputDevice->getButton("LP")) {
            // Light Punch (Jab)
            if (isCrouching && states.find("Crouch Jab") != states.end()) {
                targetState = "Crouch Jab";
            } else if (states.find("Stand Jab") != states.end()) {
                targetState = "Stand Jab";
            } else if (states.find("Jab") != states.end()) {
                targetState = "Jab";
            }
        } else if (inputDevice->getButton("MP")) {
            // Medium Punch (Strong)
            if (isCrouching && states.find("Crouch Strong") != states.end()) {
                targetState = "Crouch Strong";
            } else if (states.find("Stand Strong") != states.end()) {
                targetState = "Stand Strong";
            } else if (states.find("Strong") != states.end()) {
                targetState = "Strong";
            }
        } else if (inputDevice->getButton("HP")) {
            // Heavy Punch (Fierce)
            if (isCrouching && states.find("Crouch Fierce") != states.end()) {
                targetState = "Crouch Fierce";
            } else if (states.find("Stand Fierce") != states.end()) {
                targetState = "Stand Fierce";
            } else if (states.find("Fierce") != states.end()) {
                targetState = "Fierce";
            }
        }
        // Check for attack buttons (kicks)
        else if (inputDevice->getButton("LK")) {
            // Light Kick (Short)
            if (isCrouching && states.find("Crouch Short") != states.end()) {
                targetState = "Crouch Short";
            } else if (states.find("Stand Short") != states.end()) {
                targetState = "Stand Short";
            } else if (states.find("Short") != states.end()) {
                targetState = "Short";
            }
        } else if (inputDevice->getButton("MK")) {
            // Medium Kick (Forward)
            if (isCrouching && states.find("Crouch Forward") != states.end()) {
                targetState = "Crouch Forward";
            } else if (states.find("Stand Forward") != states.end()) {
                targetState = "Stand Forward";
            } else if (states.find("Forward") != states.end()) {
                targetState = "Forward";
            }
        } else if (inputDevice->getButton("HK")) {
            // Heavy Kick (Roundhouse)
            if (isCrouching && states.find("Crouch Roundhouse") != states.end()) {
                targetState = "Crouch Roundhouse";
            } else if (states.find("Stand Roundhouse") != states.end()) {
                targetState = "Stand Roundhouse";
            } else if (states.find("Roundhouse") != states.end()) {
                targetState = "Roundhouse";
            }
        }
        // Check for jump (only if no attack buttons pressed)
        else if (isJumping) {
            // Check for directional jumps
            if (axis[0] > 0) {
                // Forward jump
                if (states.find("Forward Jump") != states.end()) {
                    targetState = "Forward Jump";
                } else if (states.find("Neutral Jump") != states.end()) {
                    targetState = "Neutral Jump";
                }
            } else if (axis[0] < 0) {
                // Backward jump
                if (states.find("Backward Jump") != states.end()) {
                    targetState = "Backward Jump";
                } else if (states.find("Neutral Jump") != states.end()) {
                    targetState = "Neutral Jump";
                }
            } else {
                // Neutral jump
                if (states.find("Neutral Jump") != states.end()) {
                    targetState = "Neutral Jump";
                }
            }
        }
        // Check for forward/backward movement
        else if (axis[0] < 0) {
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
        // Check for crouch (only if not moving or attacking)
        else if (isCrouching && !isMoving) {
            if (states.find("Crouch") != states.end()) {
                targetState = "Crouch";
            }
        }
        
        // Transition to target state if different from current
        if (targetState != currentState && states.find(targetState) != states.end()) {
            setState(targetState);
        }
    }
    
    frame++;
}

// Load collision boxes from character JSON
void BaseActiveObject::loadBoxesFromJSON() {
    if (dict.find("json") == dict.end() || dict.at("json") == nullptr) {
        return;
    }
    
    json* jsonData = static_cast<json*>(dict.at("json"));
    
    if (!jsonData->contains("boxes")) {
        std::cout << "No boxes data in JSON" << std::endl;
        return;
    }
    
    auto& boxesData = (*jsonData)["boxes"];
    
    // Scale factor to convert from sprite space to screen space
    // Boxes in JSON are designed for sprites at 4x current screen resolution
    const float COLLISION_BOX_SCALE = 0.25f;
    
    // Load hurtbox
    if (boxesData.contains("hurtbox") && boxesData["hurtbox"].contains("boxes")) {
        for (const auto& boxArray : boxesData["hurtbox"]["boxes"]) {
            if (boxArray.is_array() && boxArray.size() >= 4) {
                // Box coordinates from JSON: [x, y, width, height]
                // JSON Y is measured from top of sprite downward
                // We need Y relative to character ground position (Y=0)
                // Scale down boxes (designed for 4x larger sprites)
                float x = boxArray[0].get<float>() * COLLISION_BOX_SCALE;
                float jsonY = boxArray[1].get<float>() * COLLISION_BOX_SCALE;
                float w = boxArray[2].get<float>() * COLLISION_BOX_SCALE;
                float h = boxArray[3].get<float>() * COLLISION_BOX_SCALE;
                
                // Convert Y from sprite-top-relative to ground-relative
                // JSON Y is distance from top of sprite downward
                // We need Y as distance from ground upward
                // Negative Y in JSON space (-jsonY) converts to upward from ground
                float y = -jsonY;
                
                defaultBoxes.hurtbox.emplace_back(x, y, w, h);
            }
        }
        std::cout << "Loaded " << defaultBoxes.hurtbox.size() << " hurtboxes" << std::endl;
    }
    
    // Load hitbox
    if (boxesData.contains("hitbox") && boxesData["hitbox"].contains("boxes")) {
        for (const auto& boxArray : boxesData["hitbox"]["boxes"]) {
            if (boxArray.is_array() && boxArray.size() >= 4) {
                float x = boxArray[0].get<float>() * COLLISION_BOX_SCALE;
                float jsonY = boxArray[1].get<float>() * COLLISION_BOX_SCALE;
                float w = boxArray[2].get<float>() * COLLISION_BOX_SCALE;
                float h = boxArray[3].get<float>() * COLLISION_BOX_SCALE;
                float y = -jsonY;
                defaultBoxes.hitbox.emplace_back(x, y, w, h);
            }
        }
        std::cout << "Loaded " << defaultBoxes.hitbox.size() << " hitboxes" << std::endl;
    }
    
    // Load pushbox (using grabbox as pushbox)
    if (boxesData.contains("grabbox") && boxesData["grabbox"].contains("boxes")) {
        for (const auto& boxArray : boxesData["grabbox"]["boxes"]) {
            if (boxArray.is_array() && boxArray.size() >= 4) {
                float x = boxArray[0].get<float>() * COLLISION_BOX_SCALE;
                float jsonY = boxArray[1].get<float>() * COLLISION_BOX_SCALE;
                float w = boxArray[2].get<float>() * COLLISION_BOX_SCALE;
                float h = boxArray[3].get<float>() * COLLISION_BOX_SCALE;
                float y = -jsonY;
                defaultBoxes.pushbox.emplace_back(x, y, w, h);
            }
        }
        std::cout << "Loaded " << defaultBoxes.pushbox.size() << " pushboxes" << std::endl;
    }
    
    // Initialize current boxes with default boxes
    currentBoxes = defaultBoxes;
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
    // Return first hurtbox if available, otherwise use default
    if (!currentBoxes.hurtbox.empty()) {
        auto& box = currentBoxes.hurtbox[0];
        // Adjust for character position and facing
        // Box Y is already inverted (negative values go up from ground)
        // Box coordinates are relative to character position
        float adjustedX = pos[0] + (face > 0 ? box.x : -box.x - box.width);
        float adjustedY = pos[1] + box.y - box.height; // Subtract height since box.y is bottom of box
        return CollisionBox(adjustedX, adjustedY, box.width, box.height);
    }
    // Fallback to default hurtbox
    return CollisionBox(pos[0] - 30.0f, pos[1] - 100.0f, 60.0f, 100.0f);
}

CollisionBox BaseActiveObject::getHitbox() const {
    // Return first hitbox if available
    if (!currentBoxes.hitbox.empty()) {
        auto& box = currentBoxes.hitbox[0];
        // Adjust for character position and facing
        float adjustedX = pos[0] + (face > 0 ? box.x : -box.x - box.width);
        float adjustedY = pos[1] + box.y - box.height;
        return CollisionBox(adjustedX, adjustedY, box.width, box.height);
    }
    // Fallback: hitbox in front of character
    float hitboxX = (face > 0) ? pos[0] : pos[0] - 50.0f;
    return CollisionBox(hitboxX, pos[1] - 60.0f, 50.0f, 40.0f);
}

CollisionBox BaseActiveObject::getPushbox() const {
    // Return first pushbox if available
    if (!currentBoxes.pushbox.empty()) {
        auto& box = currentBoxes.pushbox[0];
        // Adjust for character position and facing
        float adjustedX = pos[0] + (face > 0 ? box.x : -box.x - box.width);
        float adjustedY = pos[1] + box.y - box.height;
        return CollisionBox(adjustedX, adjustedY, box.width, box.height);
    }
    // Fallback to default pushbox
    return CollisionBox(pos[0] - 25.0f, pos[1] - 90.0f, 50.0f, 90.0f);
}

void BaseActiveObject::setState(const std::string& stateName) {
    currentState = stateName;
    frame = 0;
    animationFrame = 0;
    frameTimer = 0;
}
