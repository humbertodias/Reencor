#include "CollisionSystem.h"
#include "Game.h"
#include "BaseActiveObject.h"
#include <GL/gl.h>
#include <algorithm>

void CollisionSystem::calculateBoxCollisions(Game* game) {
    // TODO: Implement collision detection
    // This would check hitboxes, hurtboxes, etc. between active objects
    
    for (size_t i = 0; i < game->activePlayers.size(); i++) {
        for (size_t j = i + 1; j < game->activePlayers.size(); j++) {
            // Check collisions between players
            // boxCollide(player1->getHitbox(), player2->getHurtbox());
        }
    }
}

void CollisionSystem::drawBoxes(Game* game, std::shared_ptr<BaseActiveObject> object) {
    // Draw debug boxes for collision visualization
    glDisable(GL_TEXTURE_2D);
    glLineWidth(2.0f);
    
    // Draw hurtbox in green
    glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(object->pos[0] - 30, object->pos[1] - 50);
        glVertex2f(object->pos[0] + 30, object->pos[1] - 50);
        glVertex2f(object->pos[0] + 30, object->pos[1] + 50);
        glVertex2f(object->pos[0] - 30, object->pos[1] + 50);
    glEnd();
    
    // Draw hitbox in red
    glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(object->pos[0] - 20, object->pos[1] - 40);
        glVertex2f(object->pos[0] + 40, object->pos[1] - 40);
        glVertex2f(object->pos[0] + 40, object->pos[1] - 20);
        glVertex2f(object->pos[0] - 20, object->pos[1] - 20);
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
