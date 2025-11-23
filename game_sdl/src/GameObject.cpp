#include "GameObject.h"
#include "Game.h"

GameObject::GameObject(Game* game) 
    : game(game), 
      pos({0.0f, 0.0f, 0.0f}),
      type("object")
{
}
