#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include <vector>
#include <memory>

class Game;
class BaseActiveObject;

class CollisionSystem {
public:
    static void calculateBoxCollisions(Game* game);
    static void drawBoxes(Game* game, std::shared_ptr<BaseActiveObject> object);
    static bool boxCollide(const std::vector<float>& box1, const std::vector<float>& box2);
};

#endif // COLLISIONSYSTEM_H
