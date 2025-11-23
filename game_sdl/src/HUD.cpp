#include "HUD.h"
#include "Game.h"
#include "BaseActiveObject.h"
#include <iostream>
#include <algorithm>
#include <cmath>

// GaugeBar implementation
HUD::GaugeBar::GaugeBar(const GaugeConfig& config, BaseActiveObject* parent, int playerTeam)
    : config(config), parent(parent), playerTeam(playerTeam), blinkTimer(0), blinkDuration(120.0f) {}

void HUD::GaugeBar::update() {
    // Update blink timer if applicable
    if (blinkDuration > 0) {
        blinkTimer += 1.0f;
        if (blinkTimer >= blinkDuration) {
            blinkTimer = 0;
        }
    }
}

HUD::Color HUD::GaugeBar::interpolateColor(const Color& start, const Color& end, float t) const {
    t = std::clamp(t, 0.0f, 1.0f);
    return Color(
        start.r + (end.r - start.r) * t,
        start.g + (end.g - start.g) * t,
        start.b + (end.b - start.b) * t,
        start.a + (end.a - start.a) * t
    );
}

void HUD::GaugeBar::drawLine(float x1, float y1, float x2, float y2, const Color& color, float thickness) {
    glLineWidth(thickness);
    glDisable(GL_TEXTURE_2D);
    glColor4f(color.r, color.g, color.b, color.a);
    
    glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
    glEnd();
    
    glEnable(GL_TEXTURE_2D);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);  // Reset color
}

void HUD::GaugeBar::draw(float screenCenterX, float screenCenterY) {
    if (!parent) return;

    // Note: In a real implementation, we would need to get actual gauge values from parent
    // For now, we'll use placeholder values
    // TODO: Add gauge system to BaseActiveObject to track health, super meter, etc.
    
    // Placeholder: Use position as a proxy for health (will be replaced with actual gauge values)
    float gaugeValue = 0.8f; // 80% health as placeholder
    float maxValue = 1.0f;
    
    // Calculate positions based on player team (left or right side of screen)
    float xOffset = playerTeam == 1 ? -1.0f : 1.0f;
    float startX = screenCenterX + config.startPos[0] * xOffset;
    float startY = screenCenterY + config.startPos[1];
    float endX = screenCenterX + config.endPos[0] * xOffset;
    float endY = screenCenterY + config.endPos[1];
    
    // Calculate current bar end position based on gauge value
    float fillRatio = gaugeValue / maxValue;
    if (config.level > 1) {
        // For multi-level gauges (like super meter), calculate within current level
        fillRatio = std::fmod(gaugeValue * config.level, 1.0f);
        if (gaugeValue >= maxValue) {
            fillRatio = 1.0f;
        }
    }
    
    float currentEndX = startX - (startX - endX) * fillRatio * xOffset;
    float currentEndY = startY;
    
    // Interpolate color based on fill ratio
    Color barColor = interpolateColor(config.startColor, config.endColor, fillRatio);
    
    // Handle blinking effect for super meter when full
    if (blinkDuration > 0 && gaugeValue >= maxValue) {
        float blinkT = std::sin(blinkTimer / blinkDuration * 3.14159f * 2.0f) * 0.5f + 0.5f;
        barColor = interpolateColor(config.startColor, config.endColor, blinkT);
    }
    
    // Draw the gauge bar
    drawLine(startX, startY, currentEndX, currentEndY, barColor, config.thickness);
    
    // Draw level indicator if enabled
    if (config.showLevelIndicator && config.level > 1) {
        // TODO: Implement text rendering for level numbers
        // This requires font texture atlas implementation
        int currentLevel = static_cast<int>(gaugeValue * config.level);
        // Placeholder for text rendering
    }
}

// HUD implementation
HUD::HUD(Game* game) : game(game) {}

HUD::GaugeConfig HUD::createHealthBarConfig() {
    GaugeConfig config;
    // Health bar configuration matching LifeBar.json
    config.startPos = {-70.0f, 370.0f};
    config.endPos = {-550.0f, 370.0f};
    config.thickness = 10.0f;
    config.startColor = Color(255.0f/255.0f, 255.0f/255.0f, 20.0f/255.0f, 1.0f);  // Yellow when low
    config.endColor = Color(20.0f/255.0f, 255.0f/255.0f, 20.0f/255.0f, 1.0f);    // Green when full
    config.level = 1;
    config.showLevelIndicator = false;
    return config;
}

HUD::GaugeConfig HUD::createSuperBarConfig() {
    GaugeConfig config;
    // Super bar configuration matching SuperBar.json
    config.startPos = {-80.0f, -370.0f};
    config.endPos = {-520.0f, -370.0f};
    config.thickness = 12.0f;
    config.startColor = Color(80.0f/255.0f, 80.0f/255.0f, 255.0f/255.0f, 1.0f);   // Dark blue
    config.endColor = Color(126.0f/255.0f, 126.0f/255.0f, 255.0f/255.0f, 1.0f);  // Light blue
    config.level = 3;
    config.showLevelIndicator = true;
    config.levelIndicatorPos = {-590.0f, -390.0f};
    return config;
}

void HUD::initialize() {
    std::cout << "Initializing HUD system..." << std::endl;
    
    // Clear existing gauges
    healthBars.clear();
    superBars.clear();
    
    // Create gauges for each active player
    for (size_t i = 0; i < game->activePlayers.size(); i++) {
        auto& player = game->activePlayers[i];
        int team = player->team;
        
        // Create health bar
        auto healthBar = std::make_unique<GaugeBar>(createHealthBarConfig(), player.get(), team);
        healthBars.push_back(std::move(healthBar));
        
        // Create super bar
        auto superBar = std::make_unique<GaugeBar>(createSuperBarConfig(), player.get(), team);
        superBars.push_back(std::move(superBar));
        
        std::cout << "Created HUD gauges for player " << (i + 1) << " (team " << team << ")" << std::endl;
    }
    
    std::cout << "HUD initialization complete: " << healthBars.size() << " health bars, " 
              << superBars.size() << " super bars" << std::endl;
}

void HUD::update() {
    // Update all gauge bars
    for (auto& bar : healthBars) {
        bar->update();
    }
    for (auto& bar : superBars) {
        bar->update();
    }
}

void HUD::draw() {
    if (!game || !game->screen) return;
    
    // Get screen center for positioning
    float screenCenterX = game->internalResolution.first / 2.0f;
    float screenCenterY = game->internalResolution.second / 2.0f;
    
    // Draw all health bars
    for (auto& bar : healthBars) {
        bar->draw(screenCenterX, screenCenterY);
    }
    
    // Draw all super bars
    for (auto& bar : superBars) {
        bar->draw(screenCenterX, screenCenterY);
    }
}
