#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H

#include <SDL2/SDL.h>
#include <string>
#include <vector>

class Game;

class InputDevice {
public:
    enum DeviceType {
        KEYBOARD,
        JOYSTICK
    };

    InputDevice(Game* game, int playerNumber, int deviceIndex, DeviceType type);
    ~InputDevice();

    void update();
    void draw(void* screen, const std::vector<float>& cameraPos);

    bool getButton(const std::string& button) const;
    std::vector<int> getAxis() const;

private:
    Game* game;
    int playerNumber;
    int deviceIndex;
    DeviceType deviceType;
    SDL_Joystick* joystick;
    
    // Button states
    bool buttons[16];
    int axis[2];

    void updateKeyboard();
    void updateJoystick();
};

#endif // INPUTDEVICE_H
