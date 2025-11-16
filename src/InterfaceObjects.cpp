#include "InterfaceObjects.h"
#include "Game.h"

Message::Message(Game* game, const std::string& text)
    : BaseActiveObject(game), text(text), lifetime(0) {
}

void Message::update(const std::vector<float>& cameraFocusPoint) {
    BaseActiveObject::update(cameraFocusPoint);
    lifetime++;
}

void Message::draw(Screen* screen, const std::vector<float>& cameraPos) {
    // Draw message text
    BaseActiveObject::draw(screen, cameraPos);
}
