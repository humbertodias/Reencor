#include "CollisionSystem.h"
#include "Game.h"
#include "BaseActiveObject.h"
#include <GL/gl.h>
#include <algorithm>

void CollisionSystem::calculateBoxCollisions(Game* game) {
    // Calculate collisions between all active players
    for (size_t i = 0; i < game->activePlayers.size(); i++) {
        for (size_t j = i + 1; j < game->activePlayers.size(); j++) {
            auto& player1 = game->activePlayers[i];
            auto& player2 = game->activePlayers[j];
            
            // Check hitbox vs hurtbox collisions
            auto p1Hitbox = player1->getHitbox();
            auto p2Hurtbox = player2->getHurtbox();
            auto p2Hitbox = player2->getHitbox();
            auto p1Hurtbox = player1->getHurtbox();
            
            std::vector<float> p1Hit = {p1Hitbox.x, p1Hitbox.y, p1Hitbox.width, p1Hitbox.height};
            std::vector<float> p2Hurt = {p2Hurtbox.x, p2Hurtbox.y, p2Hurtbox.width, p2Hurtbox.height};
            std::vector<float> p2Hit = {p2Hitbox.x, p2Hitbox.y, p2Hitbox.width, p2Hitbox.height};
            std::vector<float> p1Hurt = {p1Hurtbox.x, p1Hurtbox.y, p1Hurtbox.width, p1Hurtbox.height};
            
            // Player 1's attack hits Player 2
            if (boxCollide(p1Hit, p2Hurt)) {
                // TODO: Apply damage/hitstun to player2
            }
            
            // Player 2's attack hits Player 1
            if (boxCollide(p2Hit, p1Hurt)) {
                // TODO: Apply damage/hitstun to player1
            }
        }
    }
}

void CollisionSystem::drawBoxes(Game* game, std::shared_ptr<BaseActiveObject> object) {
    if (!object) return;
    
    // Get collision boxes
    auto hurtbox = object->getHurtbox();
    auto hitbox = object->getHitbox();
    auto pushbox = object->getPushbox();
    
    // Calculate screen position with camera offset
    auto& cameraPos = game->camera->pos;
    float screenOffsetX = 320.0f - (cameraPos.size() > 0 ? cameraPos[0] : 0.0f);
    float screenOffsetY = 320.0f - (cameraPos.size() > 1 ? cameraPos[1] : 0.0f);
    
    glDisable(GL_TEXTURE_2D);
    glLineWidth(2.0f);
    
    // Draw hurtbox in green (character's vulnerable area)
    glColor4f(0.0f, 1.0f, 0.0f, 0.8f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(hurtbox.x + screenOffsetX, hurtbox.y + screenOffsetY);
        glVertex2f(hurtbox.x + hurtbox.width + screenOffsetX, hurtbox.y + screenOffsetY);
        glVertex2f(hurtbox.x + hurtbox.width + screenOffsetX, hurtbox.y + hurtbox.height + screenOffsetY);
        glVertex2f(hurtbox.x + screenOffsetX, hurtbox.y + hurtbox.height + screenOffsetY);
    glEnd();
    
    // Draw hitbox in red (character's attack area)
    glColor4f(1.0f, 0.0f, 0.0f, 0.8f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(hitbox.x + screenOffsetX, hitbox.y + screenOffsetY);
        glVertex2f(hitbox.x + hitbox.width + screenOffsetX, hitbox.y + screenOffsetY);
        glVertex2f(hitbox.x + hitbox.width + screenOffsetX, hitbox.y + hitbox.height + screenOffsetY);
        glVertex2f(hitbox.x + screenOffsetX, hitbox.y + hitbox.height + screenOffsetY);
    glEnd();
    
    // Draw pushbox in blue (prevents character overlap)
    glColor4f(0.0f, 0.0f, 1.0f, 0.8f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(pushbox.x + screenOffsetX, pushbox.y + screenOffsetY);
        glVertex2f(pushbox.x + pushbox.width + screenOffsetX, pushbox.y + screenOffsetY);
        glVertex2f(pushbox.x + pushbox.width + screenOffsetX, pushbox.y + pushbox.height + screenOffsetY);
        glVertex2f(pushbox.x + screenOffsetX, pushbox.y + pushbox.height + screenOffsetY);
    glEnd();
    
    glEnable(GL_TEXTURE_2D);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

bool CollisionSystem::boxCollide(const std::vector<float>& box1, const std::vector<float>& box2) {
    // Simple AABB collision detection
    // box format: [x, y, width, height]
    if (box1.size() < 4 || box2.size() < 4) return false;
    
    return (box1[0] < box2[0] + box2[2] &&
            box1[0] + box1[2] > box2[0] &&
            box1[1] < box2[1] + box2[3] &&
            box1[1] + box1[3] > box2[1]);
}
