#ifndef HUD_H
#define HUD_H

#include <GL/gl.h>
#include <memory>
#include <vector>
#include <array>

class Game;
class BaseActiveObject;

/**
 * HUD (Heads-Up Display) system for rendering UI elements like health bars and super meters.
 * This class manages all UI gauges displayed during gameplay.
 */
class HUD {
public:
    /**
     * Color structure for gauge rendering
     */
    struct Color {
        float r, g, b, a;
        Color(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f)
            : r(r), g(g), b(b), a(a) {}
    };

    /**
     * Configuration for a single gauge bar
     */
    struct GaugeConfig {
        std::array<float, 2> startPos;   // Start position [x, y]
        std::array<float, 2> endPos;     // End position [x, y]
        float thickness;                  // Bar thickness
        Color startColor;                 // Color at empty state
        Color endColor;                   // Color at full state
        int level;                        // Number of levels/segments
        bool showLevelIndicator;          // Whether to show numeric level indicator
        std::array<float, 2> levelIndicatorPos; // Position for level indicator
    };

    /**
     * Individual gauge bar for a player
     */
    class GaugeBar {
    public:
        GaugeBar(const GaugeConfig& config, BaseActiveObject* parent, int playerTeam);
        void draw(float screenCenterX, float screenCenterY);
        void update();

    private:
        GaugeConfig config;
        BaseActiveObject* parent;
        int playerTeam;
        float blinkTimer;
        float blinkDuration;

        Color interpolateColor(const Color& start, const Color& end, float t) const;
        void drawLine(float x1, float y1, float x2, float y2, const Color& color, float thickness);
    };

    HUD(Game* game);
    ~HUD() = default;

    /**
     * Initialize HUD gauges for all active players
     */
    void initialize();

    /**
     * Update all HUD elements
     */
    void update();

    /**
     * Render all HUD elements
     */
    void draw();

private:
    Game* game;
    std::vector<std::unique_ptr<GaugeBar>> healthBars;
    std::vector<std::unique_ptr<GaugeBar>> superBars;

    // Configuration for health and super bars
    GaugeConfig createHealthBarConfig();
    GaugeConfig createSuperBarConfig();
};

#endif // HUD_H
