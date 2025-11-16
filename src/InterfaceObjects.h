#ifndef INTERFACE_OBJECTS_H
#define INTERFACE_OBJECTS_H

#include "BaseActiveObject.h"
#include <string>

class Message : public BaseActiveObject {
public:
    Message(Game* game, const std::string& text);
    void update(const std::vector<float>& cameraFocusPoint) override;
    void draw(Screen* screen, const std::vector<float>& cameraPos) override;
    
private:
    std::string text;
    int lifetime;
};

#endif // INTERFACE_OBJECTS_H
