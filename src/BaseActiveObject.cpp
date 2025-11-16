#include "BaseActiveObject.h"
#include "Game.h"
#include "OpenGLRenderer.h"

BaseActiveObject::BaseActiveObject(
    Game* game,
    const std::string& type,
    const std::map<std::string, std::string>& dict,
    int team,
    InputDevice* inputDevice,
    const std::vector<float>& pos,
    int face,
    int palette,
    const std::string& initialState
) : game(game), type(type), dict(dict), team(team), inputDevice(inputDevice),
    pos(pos), face(face), palette(palette), currentState(initialState), stateFrame(0) {
    
    // Initialize visual properties
    imageOffset = {0, 0};
    imageSize = {100, 100};
    imageMirror = {false, false};
    imageTint = {255, 255, 255, 255};
    imageAngle = {0, 0, 0};
    imageRepeat = false;
    imageGlow = 0.0f;
}

void BaseActiveObject::update(const std::vector<float>& cameraFocusPoint) {
    // Update object state
    stateFrame++;
    
    // Basic update logic would go here
    // This would include:
    // - Processing input
    // - Updating physics
    // - State transitions
    // - Animation updates
}

void BaseActiveObject::draw(Screen* screen, const std::vector<float>& cameraPos) {
    // Draw the object
    if (!image.empty() && game->imageDict.find(image) != game->imageDict.end()) {
        const auto& texData = game->imageDict[image];
        
        std::vector<float> drawPos = {
            pos[0] + imageOffset[0],
            pos[1] + imageOffset[1],
            pos.size() > 2 ? pos[2] : 0.0f
        };
        
        std::vector<bool> flip = {face < 0, false};
        
        drawTexture(
            texData.id,
            drawPos,
            imageSize,
            flip,
            imageTint,
            imageAngle,
            imageRepeat,
            imageGlow,
            false,
            false
        );
    }
}
