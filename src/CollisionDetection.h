#ifndef COLLISION_DETECTION_H
#define COLLISION_DETECTION_H

class Game;
class BaseActiveObject;

void calculateBoxesCollisions(Game* game);
void drawBoxes(Game* game, BaseActiveObject* object);
bool boxCollide(const BaseActiveObject* obj1, const BaseActiveObject* obj2);

#endif // COLLISION_DETECTION_H
