#include "ComboTrial.h"
#include "Game.h"
#include "BaseActiveObject.h"
#include "Renderer.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <GL/gl.h>

using json = nlohmann::json;

ComboTrial::ComboTrial(Game* game, const std::string& characterName, int trialLevel)
    : game(game), characterName(characterName), trialLevel(trialLevel),
      currentMoveIndex(0), trialCompleted(false) {
    
    if (loadTrialsFromJSON()) {
        if (trialLevel < allTrials.size()) {
            currentTrial = allTrials[trialLevel];
            std::cout << "Loaded combo trial: " << currentTrial.title << std::endl;
        }
    }
}

bool ComboTrial::loadTrialsFromJSON() {
    // Try to load character JSON data
    std::string jsonPath = "../Assets/objects/" + characterName + ".json";
    std::ifstream file(jsonPath);
    
    if (!file.is_open()) {
        std::cerr << "Failed to open character JSON: " << jsonPath << std::endl;
        return false;
    }
    
    try {
        json charData;
        file >> charData;
        
        // Check if combo_trails exists
        if (!charData.contains("combo_trails")) {
            std::cout << "No combo_trails found in character JSON" << std::endl;
            return false;
        }
        
        auto& trials = charData["combo_trails"];
        if (!trials.is_array()) {
            std::cerr << "combo_trails is not an array" << std::endl;
            return false;
        }
        
        // Parse each trial
        for (const auto& trialJson : trials) {
            ComboTrialData trial;
            
            trial.title = trialJson.value("title", "Untitled Trial");
            trial.difficulty = trialJson.value("difficulty", 1);
            trial.reward = trialJson.value("reward", 1);
            
            // Parse start positions
            if (trialJson.contains("start_pos") && trialJson["start_pos"].is_array()) {
                for (const auto& posArray : trialJson["start_pos"]) {
                    std::vector<float> pos;
                    for (const auto& val : posArray) {
                        pos.push_back(val.get<float>());
                    }
                    trial.startPos.push_back(pos);
                }
            }
            
            // Parse sequence
            if (trialJson.contains("sequence") && trialJson["sequence"].is_array()) {
                for (const auto& moveJson : trialJson["sequence"]) {
                    ComboMove move;
                    move.input = moveJson.value("input", "");
                    move.move = moveJson.value("move", "");
                    move.comment = moveJson.value("comment", "");
                    move.requiresHit = moveJson.value("hit", true);
                    trial.sequence.push_back(move);
                }
            }
            
            allTrials.push_back(trial);
        }
        
        std::cout << "Loaded " << allTrials.size() << " combo trials for " << characterName << std::endl;
        return !allTrials.empty();
        
    } catch (const json::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return false;
    }
}

void ComboTrial::update() {
    if (trialCompleted || game->activePlayers.empty()) {
        return;
    }
    
    // Check if current move was completed
    if (checkMoveCompletion()) {
        std::cout << "Move " << currentMoveIndex << " completed: " 
                  << currentTrial.sequence[currentMoveIndex].move << std::endl;
        
        currentMoveIndex++;
        
        // Check if entire trial is completed
        if (currentMoveIndex >= currentTrial.sequence.size()) {
            trialCompleted = true;
            std::cout << "Trial completed: " << currentTrial.title << std::endl;
        }
    }
}

bool ComboTrial::checkMoveCompletion() {
    if (currentMoveIndex >= currentTrial.sequence.size()) {
        return false;
    }
    
    if (game->activePlayers.empty()) {
        return false;
    }
    
    auto& player = game->activePlayers[0];
    const auto& currentMove = currentTrial.sequence[currentMoveIndex];
    
    // TODO: Implement proper move checking with input sequence validation
    // Current implementation is a simplified stub that only checks state name
    // Full implementation should:
    // 1. Track input history and validate input sequences match trial requirements
    // 2. Check combo_list array for hit confirmation when requiresHit is true
    // 3. Validate move timing and cancels
    // 4. Handle special cases like counters, parries, etc.
    
    if (currentMove.requiresHit) {
        // Check if the move hit the opponent (simplified check)
        // In full implementation, check combo_list for proper hit confirmation
        return player->currentState == currentMove.move;
    } else {
        // Just check if the state was entered
        return player->currentState == currentMove.move;
    }
}

void ComboTrial::draw() {
    if (game->activePlayers.empty()) return;
    
    // Draw trial title
    glDisable(GL_TEXTURE_2D);
    glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
    
    // Draw trial information at top left
    float startX = 50.0f;
    float startY = 80.0f;
    
    // Draw trial name background
    glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
    glBegin(GL_QUADS);
        glVertex2f(startX - 10, startY - 5);
        glVertex2f(startX + 300, startY - 5);
        glVertex2f(startX + 300, startY + 25);
        glVertex2f(startX - 10, startY + 25);
    glEnd();
    
    // Draw progress indicator for each move in sequence
    float moveY = startY + 40;
    for (size_t i = 0; i < currentTrial.sequence.size(); i++) {
        // Set color based on completion status
        if (i < currentMoveIndex) {
            glColor4f(0.0f, 1.0f, 0.0f, 0.8f);  // Green for completed
        } else if (i == currentMoveIndex) {
            glColor4f(1.0f, 1.0f, 0.0f, 0.8f);  // Yellow for current
        } else {
            glColor4f(0.5f, 0.5f, 0.5f, 0.6f);  // Gray for pending
        }
        
        // Draw move indicator box
        glBegin(GL_QUADS);
            glVertex2f(startX, moveY);
            glVertex2f(startX + 250, moveY);
            glVertex2f(startX + 250, moveY + 30);
            glVertex2f(startX, moveY + 30);
        glEnd();
        
        // Draw border
        glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(startX, moveY);
            glVertex2f(startX + 250, moveY);
            glVertex2f(startX + 250, moveY + 30);
            glVertex2f(startX, moveY + 30);
        glEnd();
        
        moveY += 35;
    }
    
    // Draw completion status
    if (trialCompleted) {
        float centerX = 320.0f;
        float centerY = 200.0f;
        
        glColor4f(0.0f, 1.0f, 0.0f, 0.9f);
        glBegin(GL_QUADS);
            glVertex2f(centerX - 150, centerY - 30);
            glVertex2f(centerX + 150, centerY - 30);
            glVertex2f(centerX + 150, centerY + 30);
            glVertex2f(centerX - 150, centerY + 30);
        glEnd();
        
        // Draw "COMPLETE!" border
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glLineWidth(4.0f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(centerX - 150, centerY - 30);
            glVertex2f(centerX + 150, centerY - 30);
            glVertex2f(centerX + 150, centerY + 30);
            glVertex2f(centerX - 150, centerY + 30);
        glEnd();
    }
    
    glEnable(GL_TEXTURE_2D);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void ComboTrial::reset() {
    currentMoveIndex = 0;
    trialCompleted = false;
    
    // Reset player positions if trial specifies start positions
    if (currentTrial.startPos.size() >= 2 && game->activePlayers.size() >= 2) {
        if (currentTrial.startPos[0].size() >= 2) {
            game->activePlayers[0]->pos[0] = currentTrial.startPos[0][0];
            // Y position handled by rendering offset
        }
        if (currentTrial.startPos[1].size() >= 2) {
            game->activePlayers[1]->pos[0] = currentTrial.startPos[1][0];
        }
    }
}

bool ComboTrial::loadNextTrial() {
    trialLevel++;
    if (trialLevel < allTrials.size()) {
        currentTrial = allTrials[trialLevel];
        currentMoveIndex = 0;
        trialCompleted = false;
        std::cout << "Loaded next trial: " << currentTrial.title << std::endl;
        reset();
        return true;
    }
    return false;
}
