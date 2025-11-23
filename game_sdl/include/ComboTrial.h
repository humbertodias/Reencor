#ifndef COMBOTRIAL_H
#define COMBOTRIAL_H

#include <string>
#include <vector>
#include <memory>

class Game;
class BaseActiveObject;

/**
 * Represents a single move in a combo sequence
 */
struct ComboMove {
    std::string input;      // Input notation (e.g., "QCF,>,p_b6")
    std::string move;       // Move name (e.g., "SF3/Hadouken Jab")
    std::string comment;    // Optional comment
    bool requiresHit;       // Whether this move must hit the opponent (default: true)
    
    ComboMove() : requiresHit(true) {}
};

/**
 * Represents a complete combo trial
 */
struct ComboTrialData {
    std::string title;              // Trial name (e.g., "Hadouken")
    int difficulty;                 // Difficulty level (1-5)
    int reward;                     // Reward points
    std::vector<std::vector<float>> startPos;  // Starting positions [player1, player2]
    std::vector<ComboMove> sequence;          // Sequence of moves to perform
    
    ComboTrialData() : difficulty(1), reward(1) {}
};

/**
 * Manages combo trial system including loading trials,
 * tracking progress, and validating player performance
 */
class ComboTrial {
public:
    ComboTrial(Game* game, const std::string& characterName, int trialLevel = 0);
    ~ComboTrial() = default;
    
    /**
     * Update trial state (check for move completion, etc.)
     */
    void update();
    
    /**
     * Draw trial information on screen
     */
    void draw();
    
    /**
     * Get current trial data
     */
    const ComboTrialData& getCurrentTrial() const { return currentTrial; }
    
    /**
     * Get current move index in sequence
     */
    int getCurrentMoveIndex() const { return currentMoveIndex; }
    
    /**
     * Check if trial is completed
     */
    bool isCompleted() const { return trialCompleted; }
    
    /**
     * Reset current trial
     */
    void reset();
    
    /**
     * Load next trial
     */
    bool loadNextTrial();
    
private:
    Game* game;
    std::string characterName;
    int trialLevel;
    int currentMoveIndex;
    bool trialCompleted;
    
    std::vector<ComboTrialData> allTrials;
    ComboTrialData currentTrial;
    
    // Helper to load trials from character JSON
    bool loadTrialsFromJSON();
    
    // Check if current move matches player action
    bool checkMoveCompletion();
};

#endif // COMBOTRIAL_H
