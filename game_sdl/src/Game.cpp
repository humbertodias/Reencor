#include "Game.h"
#include "Camera.h"
#include "Screen.h"
#include "InputDevice.h"
#include "GameScreen.h"
#include "AssetLoader.h"
#include "Renderer.h"
#include <SDL2/SDL_mixer.h>
#include <iostream>

Game::Game() 
    : type("game"),
      resolution(640, 400),
      internalResolution(1280, 800),
      frameRate(60),
      emuFrame(0),
      hitstop(0),
      superstop(0),
      showBoxes(false),
      showInputs(false),
      active(true),
      playerNumber(2),
      window(nullptr),
      glContext(nullptr)
{
    cameraFocusPoint = {0.0f, 0.0f, -400.0f};
    pos = {10.0f, 0.0f, 0.0f};
    selectedCharacters = {"SF3/Ryu", "SF3/Ken"};
    selectedStage = "Reencor/Training";
}

Game::~Game() {
    shutdown();
}

bool Game::initialize() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Create window
    window = SDL_CreateWindow("REENCOR",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              resolution.first,
                              resolution.second,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create OpenGL context
    glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "OpenGL context creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // Initialize OpenGL
    Renderer::initOpenGL(resolution.first, resolution.second);

    // Initialize audio
    initAudio();

    // Initialize input devices
    inputDeviceAvailable();

    // Initialize camera and screen
    camera = std::make_shared<Camera>(0.1f);
    screen = std::make_shared<Screen>(internalResolution);

    // Load assets
    loadAssets();

    return true;
}

void Game::initAudio() {
    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 1, 1024) < 0) {
        std::cerr << "SDL_mixer initialization failed: " << Mix_GetError() << std::endl;
    }
    Mix_AllocateChannels(16);
}

void Game::loadAssets() {
    std::string assetsPath = "../Assets";
    AssetLoader::loadAssets(assetsPath, imageDict, soundDict, objectDict);
}

void Game::inputDeviceAvailable() {
    // Add keyboard device
    inputDeviceList.push_back(
        std::make_shared<InputDevice>(this, 1, 0, InputDevice::KEYBOARD)
    );

    // Add joystick devices
    int joystickCount = SDL_NumJoysticks();
    for (int i = 0; i < joystickCount; i++) {
        inputDeviceList.push_back(
            std::make_shared<InputDevice>(this, 2, i, InputDevice::JOYSTICK)
        );
    }
}

void Game::run() {
    // Initialize screen sequence
    screenSequence.push_back(std::make_shared<ComboTrialScreen>(this));
    
    screenManager();
}

void Game::screenManager() {
    while (!screenSequence.empty()) {
        active = true;
        currentScreen = screenSequence.back();
        
        Uint32 lastTime = SDL_GetTicks();
        const Uint32 frameDelay = 1000 / frameRate;
        
        while (active) {
            Uint32 frameStart = SDL_GetTicks();
            
            // Update input devices
            for (auto& dev : inputDeviceList) {
                dev->update();
            }
            
            // Update camera
            camera->update(cameraFocusPoint);
            
            // Run current screen loop
            currentScreen->loop();
            
            // Display
            screen->display();
            SDL_GL_SwapWindow(window);
            
            // Handle events
            eventHandler();
            
            // Frame rate control
            Uint32 frameTime = SDL_GetTicks() - frameStart;
            if (frameDelay > frameTime) {
                SDL_Delay(frameDelay - frameTime);
            }
        }
        
        currentScreen->deinit();
        screenSequence.pop_back();
        
        // Reset state
        hitstop = 0;
        cameraFocusPoint = {0.0f, 0.0f, -400.0f};
        superstop = 0;
        pos = {10.0f, 0.0f, 0.0f};
        showBoxes = false;
        active = true;
        activePlayers.clear();
        activeStages = nullptr;
    }
}

void Game::eventHandler() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                active = false;
                screenSequence.clear();
                break;
                
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_0) {
                    active = false;
                    screenSequence.clear();
                }
                if (event.key.keysym.sym == SDLK_9) {
                    active = false;
                }
                break;
                
            case SDL_JOYDEVICEADDED:
                {
                    int deviceIndex = event.jdevice.which;
                    inputDeviceList.push_back(
                        std::make_shared<InputDevice>(this, 2, deviceIndex, InputDevice::JOYSTICK)
                    );
                }
                break;
        }
    }
}

void Game::gameplay() {
    emuFrame++;
    
    for (auto& object : objectList) {
        object->update(cameraFocusPoint);
    }
    
    if (hitstop > 0) {
        hitstop--;
    }
    
    calculateCameraFocusPoint();
}

void Game::display() {
    for (auto& object : objectList) {
        object->draw(screen.get(), camera->pos);
    }
}

void Game::calculateCameraFocusPoint() {
    if (activePlayers.empty()) {
        return;
    }

    // Calculate average position of active players
    float sumX = 0.0f, sumY = 0.0f;
    for (const auto& player : activePlayers) {
        sumX += player->pos[0];
        sumY += player->pos[1];
    }
    
    pos[0] = sumX / activePlayers.size();
    pos[1] = (sumY / activePlayers.size()) + resolution.second * 0.6f;
    pos[2] = 400.0f;
    
    cameraFocusPoint = pos;
}

void Game::nextScreen(const std::vector<std::string>& newScreenSequence) {
    active = false;
    // Add new screens to sequence
    // This would need proper screen factory implementation
}

void Game::shutdown() {
    // Clean up SDL_mixer
    for (auto& sound : soundDict) {
        if (sound.second) {
            Mix_FreeChunk(sound.second);
        }
    }
    Mix_CloseAudio();
    
    // Clean up OpenGL textures
    for (auto& img : imageDict) {
        if (img.second) {
            glDeleteTextures(1, &img.second);
        }
    }
    
    // Clean up SDL
    if (glContext) {
        SDL_GL_DeleteContext(glContext);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}
