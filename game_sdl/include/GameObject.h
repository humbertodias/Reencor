#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <vector>
#include <memory>
#include <string>

class Game;

class GameObject {
public:
    GameObject(Game* game);
    virtual ~GameObject() = default;

    virtual void update(const std::vector<float>& cameraFocusPoint) = 0;
    virtual void draw(void* screen, const std::vector<float>& cameraPos) = 0;

    std::vector<float> pos;
    std::string type;
    
protected:
    Game* game;
};

#endif // GAMEOBJECT_H
