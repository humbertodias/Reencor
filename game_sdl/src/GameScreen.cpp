#include "GameScreen.h"
#include "Game.h"
#include "BaseActiveObject.h"

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
}

void ComboTrialScreen::loadObjects() {
    // Create stage
    if (game->objectDict.find(game->selectedStage) != game->objectDict.end()) {
        // game->activeStages = std::make_shared<BaseActiveObject>(...);
    }
    
    // Create players
    for (size_t i = 0; i < game->selectedCharacters.size() && i < 2; i++) {
        if (game->objectDict.find(game->selectedCharacters[i]) != game->objectDict.end()) {
            auto player = std::make_shared<BaseActiveObject>(
                game,
                std::unordered_map<std::string, void*>(), // Empty dict for now
                std::vector<float>{i == 0 ? -300.0f : 300.0f, -1.0f, 0.0f},
                i == 0 ? 1 : -1,
                i < game->inputDeviceList.size() ? game->inputDeviceList[i] : nullptr,
                i + 1
            );
            game->activePlayers.push_back(player);
            game->objectList.push_back(player);
        }
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
