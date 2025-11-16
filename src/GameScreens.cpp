#include "GameScreens.h"
#include "Game.h"

GameScreen::GameScreen(Game* game) : game(game) {}

TitleScreen::TitleScreen(Game* game) : GameScreen(game) {}

void TitleScreen::loop() {
    // Title screen logic
}

void TitleScreen::deinit() {
    // Cleanup
}

ModeSelectionScreen::ModeSelectionScreen(Game* game) : GameScreen(game) {}

void ModeSelectionScreen::loop() {
    // Mode selection logic
}

void ModeSelectionScreen::deinit() {
    // Cleanup
}

ComboTrialScreen::ComboTrialScreen(Game* game) : GameScreen(game) {}

void ComboTrialScreen::loop() {
    // Combo trial screen logic
    game->gameplay();
    game->display();
}

void ComboTrialScreen::deinit() {
    // Cleanup
}
