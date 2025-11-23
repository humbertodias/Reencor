#include "InputDevice.h"
#include "Game.h"
#include <cstring>

InputDevice::InputDevice(Game* game, int playerNumber, int deviceIndex, DeviceType type)
    : game(game),
      playerNumber(playerNumber),
      deviceIndex(deviceIndex),
      deviceType(type),
      joystick(nullptr)
{
    memset(buttons, 0, sizeof(buttons));
    axis[0] = 0;
    axis[1] = 0;

    if (deviceType == JOYSTICK) {
        joystick = SDL_JoystickOpen(deviceIndex);
        if (!joystick) {
            // Fall back to keyboard
            deviceType = KEYBOARD;
        }
    }
}

InputDevice::~InputDevice() {
    if (joystick) {
        SDL_JoystickClose(joystick);
    }
}

void InputDevice::update() {
    if (deviceType == KEYBOARD) {
        updateKeyboard();
    } else {
        updateJoystick();
    }
}

void InputDevice::updateKeyboard() {
    const Uint8* keyState = SDL_GetKeyboardState(nullptr);
    
    // Direction keys
    buttons[0] = keyState[SDL_SCANCODE_LEFT];   // Left
    buttons[1] = keyState[SDL_SCANCODE_RIGHT];  // Right
    buttons[2] = keyState[SDL_SCANCODE_DOWN];   // Down
    buttons[3] = keyState[SDL_SCANCODE_UP];     // Up
    
    // Attack buttons
    buttons[4] = keyState[SDL_SCANCODE_A];  // Light Punch
    buttons[5] = keyState[SDL_SCANCODE_S];  // Medium Punch
    buttons[6] = keyState[SDL_SCANCODE_D];  // Heavy Punch
    buttons[7] = keyState[SDL_SCANCODE_Q];  // Light Kick
    buttons[8] = keyState[SDL_SCANCODE_W];  // Medium Kick
    buttons[9] = keyState[SDL_SCANCODE_E];  // Heavy Kick
    
    // Convert to axis
    axis[0] = 0;
    axis[1] = 0;
    if (buttons[0]) axis[0] = -1;  // Left
    if (buttons[1]) axis[0] = 1;   // Right
    if (buttons[2]) axis[1] = 1;   // Down
    if (buttons[3]) axis[1] = -1;  // Up
}

void InputDevice::updateJoystick() {
    if (!joystick) return;
    
    // Update axis
    axis[0] = SDL_JoystickGetAxis(joystick, 0) / 32767;
    axis[1] = SDL_JoystickGetAxis(joystick, 1) / 32767;
    
    // Update buttons
    int numButtons = SDL_JoystickNumButtons(joystick);
    for (int i = 0; i < numButtons && i < 16; i++) {
        buttons[i] = SDL_JoystickGetButton(joystick, i);
    }
}

bool InputDevice::getButton(const std::string& button) const {
    // Simple button mapping
    if (button == "LP") return buttons[4];
    if (button == "MP") return buttons[5];
    if (button == "HP") return buttons[6];
    if (button == "LK") return buttons[7];
    if (button == "MK") return buttons[8];
    if (button == "HK") return buttons[9];
    return false;
}

std::vector<int> InputDevice::getAxis() const {
    return {axis[0], axis[1]};
}

void InputDevice::draw(void* screen, const std::vector<float>& cameraPos) {
    // TODO: Implement input display for debugging
}
