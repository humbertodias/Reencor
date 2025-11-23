#include "GameScreen.h"
#include "Game.h"
#include "BaseActiveObject.h"
#include <iostream>

// Base GameScreen
GameScreen::GameScreen(Game* game) : game(game) {}

// TitleScreen
TitleScreen::TitleScreen(Game* game) : GameScreen(game) {}

void TitleScreen::loop() {
    // TODO: Implement title screen logic
}

void TitleScreen::deinit() {
    // Cleanup
}

// ModeSelectionScreen
ModeSelectionScreen::ModeSelectionScreen(Game* game) : GameScreen(game) {}

void ModeSelectionScreen::loop() {
    // TODO: Implement mode selection logic
}

void ModeSelectionScreen::deinit() {
    // Cleanup
}

// ComboTrialScreen
ComboTrialScreen::ComboTrialScreen(Game* game) : GameScreen(game) {
    loadObjects();
    
    // Initialize HUD after players are loaded
    if (game->hud) {
        game->hud->initialize();
    }
}

void ComboTrialScreen::loadObjects() {
    // Create stage
    if (game->objectDict.find(game->selectedStage) != game->objectDict.end()) {
        // game->activeStages = std::make_shared<BaseActiveObject>(...);
        std::cout << "Stage found: " << game->selectedStage << std::endl;
    } else {
        std::cout << "Stage not found: " << game->selectedStage << " (will continue without stage)" << std::endl;
    }
    
    // Create players with JSON data
    for (size_t i = 0; i < game->selectedCharacters.size() && i < 2; i++) {
        std::shared_ptr<InputDevice> inputDev = nullptr;
        if (i < game->inputDeviceList.size()) {
            inputDev = game->inputDeviceList[i];
        }
        
        // Build dict for player with JSON data
        std::unordered_map<std::string, void*> playerDict;
        
        // Look for JSON in objectDict
        if (game->objectDict.find(game->selectedCharacters[i]) != game->objectDict.end()) {
            playerDict["json"] = game->objectDict[game->selectedCharacters[i]];
            std::cout << "Using JSON for character: " << game->selectedCharacters[i] << std::endl;
        }
        
        auto player = std::make_shared<BaseActiveObject>(
            game,
            playerDict,
            std::vector<float>{i == 0 ? -300.0f : 300.0f, 0.0f, 0.0f},  // Y=0 for ground level
            i == 0 ? 1 : -1,
            inputDev,
            i + 1
        );
        game->activePlayers.push_back(player);
        game->objectList.push_back(player);
        
        std::cout << "Created player " << (i+1) << " at position " 
                  << (i == 0 ? -300.0f : 300.0f) << std::endl;
    }
}

void ComboTrialScreen::loop() {
    game->gameplay();
    game->display();
}

void ComboTrialScreen::deinit() {
    // Cleanup
    game->objectList.clear();
}
