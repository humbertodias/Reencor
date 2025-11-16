#include "InputDevice.h"
#include "Game.h"
#include <iostream>

// Keyboard mapping (matching Python version)
const std::vector<std::vector<int>> keyboardMapping = {
    {79},      // Left
    {80},      // Right
    {82},      // Up
    {81},      // Down
    {8},       // Light Punch
    {26},      // Medium Punch
    {20},      // Heavy Punch
    {7},       // Light Kick
    {22},      // Medium Kick
    {4},       // Heavy Kick
    {21, 92},  // Special
    {21},      // Extra
    {116}      // Debug
};

InputDevice::InputDevice(Game* game, int team, int index, const std::string& mode)
    : game(game), team(team), type("input"), controller(nullptr), activeObject(nullptr) {
    
    // Initialize input vectors
    currentInput = {{0, 0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}};
    rawInput = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    lastInput = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    pressCharge = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    interPress = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    
    // Set mode function pointer
    if (mode == "keyboard") {
        modeFunction = &InputDevice::keyboardMode;
    } else if (mode == "joystick") {
        modeFunction = &InputDevice::joystickMode;
        if (SDL_NumJoysticks() > index) {
            controller = SDL_JoystickOpen(index);
            if (controller) {
                controllerName = SDL_JoystickName(controller);
            }
        }
    } else if (mode == "AI") {
        modeFunction = &InputDevice::AIMode;
    } else if (mode == "record") {
        modeFunction = &InputDevice::recordMode;
    } else if (mode == "random") {
        modeFunction = &InputDevice::randomMode;
    } else {
        modeFunction = &InputDevice::noneMode;
    }
}

InputDevice::~InputDevice() {
    if (controller) {
        SDL_JoystickClose(controller);
    }
}

void InputDevice::update() {
    if (modeFunction) {
        (this->*modeFunction)();
    }
}

void InputDevice::keyboardMode() {
    const Uint8* keyState = SDL_GetKeyboardState(nullptr);
    
    // Update raw input from keyboard
    for (size_t i = 0; i < keyboardMapping.size() && i < rawInput.size(); ++i) {
        bool pressed = false;
        for (int scancode : keyboardMapping[i]) {
            if (keyState[scancode]) {
                pressed = true;
                break;
            }
        }
        rawInput[i] = pressed ? 1.0f : 0.0f;
    }
    
    // Update directional input
    if (rawInput.size() >= 4) {
        currentInput[0][0] = rawInput[1] - rawInput[0]; // Right - Left
        currentInput[0][1] = rawInput[2] - rawInput[3]; // Up - Down
    }
    
    // Update button inputs
    for (size_t i = 4; i < rawInput.size() && i < currentInput.size(); ++i) {
        if (currentInput[i].empty()) {
            currentInput[i].push_back(0);
        }
        currentInput[i][0] = rawInput[i];
    }
}

void InputDevice::joystickMode() {
    if (!controller) return;
    
    // Get joystick state
    // Analog sticks
    if (SDL_JoystickNumAxes(controller) >= 2) {
        float axisX = SDL_JoystickGetAxis(controller, 0) / 32768.0f;
        float axisY = SDL_JoystickGetAxis(controller, 1) / 32768.0f;
        
        currentInput[0][0] = std::abs(axisX) > 0.2f ? axisX : 0.0f;
        currentInput[0][1] = std::abs(axisY) > 0.2f ? -axisY : 0.0f; // Invert Y
    }
    
    // Buttons
    int numButtons = SDL_JoystickNumButtons(controller);
    for (int i = 0; i < numButtons && i + 1 < static_cast<int>(currentInput.size()); ++i) {
        if (currentInput[i + 1].empty()) {
            currentInput[i + 1].push_back(0);
        }
        currentInput[i + 1][0] = SDL_JoystickGetButton(controller, i) ? 1.0f : 0.0f;
    }
}

void InputDevice::AIMode() {
    // AI input logic would go here
    // For now, do nothing
}

void InputDevice::recordMode() {
    // Record input logic would go here
    keyboardMode(); // Fall back to keyboard for now
}

void InputDevice::noneMode() {
    // No input
    for (auto& input : currentInput) {
        for (auto& val : input) {
            val = 0.0f;
        }
    }
}

void InputDevice::randomMode() {
    // Random input logic would go here
    noneMode(); // Fall back to none for now
}

void InputDevice::draw(Screen* screen, const std::vector<float>& cameraPos) {
    // Draw input display if needed
    // This would show the current inputs on screen
}
