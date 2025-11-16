#include "Game.h"
#include "OpenGLRenderer.h"
#include "InputDevice.h"
#include "BaseActiveObject.h"
#include "GameScreens.h"
#include "CollisionDetection.h"
#include "CommonFunctions.h"
#include <SDL2/SDL_image.h>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

Game::Game()
    : type("game"), window(nullptr), glContext(nullptr),
      resolution{640, 400}, internalResolution{1280, 800},
      frameRate(60), emuFrame(0), hitstop(0),
      superstop(0), showBoxes(false), showInputs(false),
      active(true), playerNumber(2), running(false),
      lastFrameTime(0), recordInput(false), reproduceInput(false) {
    
    cameraFocusPoint = {0.0f, 0.0f, -400.0f};
    frame = {0, 0};
    pos = {10.0f, 0.0f, 0.0f};
    drawShake = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    
    selectedCharacters = {"ryu SF3", "ryu SF3"};
    selectedStage = "training stage";
}

Game::~Game() {
    cleanup();
}

bool Game::initialize() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Initialize SDL_image
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image initialization failed: " << IMG_GetError() << std::endl;
        return false;
    }
    
    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 1, 1024) < 0) {
        std::cerr << "SDL_mixer initialization failed: " << Mix_GetError() << std::endl;
        return false;
    }
    Mix_AllocateChannels(16);
    
    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    
    // Create window
    window = SDL_CreateWindow(
        "REENCOR",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        resolution.first,
        resolution.second,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    
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
    
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        std::cerr << "GLEW initialization failed: " << glewGetErrorString(glewError) << std::endl;
        return false;
    }
    
    // Set up OpenGL
    setModeOpenGL(resolution.first, resolution.second);
    
    // Create camera and screen
    camera = std::make_unique<Camera>(0.1f);
    screen = std::make_unique<Screen>(internalResolution.first, internalResolution.second);
    
    // Load resources
    loadDictionaries();
    
    // Set up input devices
    inputDeviceAvailable();
    
    // Set up initial screen sequence
    screenSequence = {"ComboTrialScreen"};
    selectedCharacters = {"SF3/Ryu", "SF3/Ken"};
    selectedStage = "Reencor/Training";
    
    running = true;
    return true;
}

void Game::loadDictionaries() {
    std::string assetsPath = "Assets";
    
    // Load images
    for (const auto& entry : fs::recursive_directory_iterator(assetsPath)) {
        if (entry.is_regular_file()) {
            std::string path = entry.path().string();
            std::string ext = entry.path().extension().string();
            
            // Get key from path
            std::string key = entry.path().stem().string();
            if (entry.path().parent_path().filename() != "Assets") {
                key = entry.path().parent_path().filename().string() + "/" + key;
            }
            
            if (ext == ".png" || ext == ".jpg" || ext == ".jpeg") {
                try {
                    TextureData texData = loadImagePath(path);
                    if (texData.id != 0) {
                        imageDict[key] = texData;
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Failed to load image " << path << ": " << e.what() << std::endl;
                }
            } else if (ext == ".wav" || ext == ".ogg" || ext == ".mp3") {
                try {
                    Mix_Chunk* sound = Mix_LoadWAV(path.c_str());
                    if (sound) {
                        soundDict[key] = sound;
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Failed to load sound " << path << ": " << e.what() << std::endl;
                }
            } else if (ext == ".json") {
                try {
                    std::ifstream file(path);
                    std::string content((std::istreambuf_iterator<char>(file)),
                                       std::istreambuf_iterator<char>());
                    objectDict[key] = content;
                } catch (const std::exception& e) {
                    std::cerr << "Failed to load JSON " << path << ": " << e.what() << std::endl;
                }
            }
        }
    }
    
    std::cout << "Loaded " << imageDict.size() << " images, "
              << soundDict.size() << " sounds, "
              << objectDict.size() << " objects" << std::endl;
}

void Game::inputDeviceAvailable() {
    // Add keyboard input device
    auto keyboardDevice = std::make_shared<InputDevice>(this, 1, 0, "keyboard");
    inputDeviceList.push_back(keyboardDevice);
    
    // Add joystick devices
    int numJoysticks = SDL_NumJoysticks();
    for (int i = 0; i < numJoysticks; ++i) {
        auto joystickDevice = std::make_shared<InputDevice>(this, 2, i, "joystick");
        inputDeviceList.push_back(joystickDevice);
    }
    
    // Create dummy input device
    dummyInputDevice = std::make_shared<InputDevice>(this, 2, 0, "none");
}

void Game::run() {
    screenManager();
}

void Game::screenManager() {
    while (!screenSequence.empty() && running) {
        active = true;
        
        // Create current screen
        if (screenSequence.back() == "ComboTrialScreen") {
            currentScreen = std::make_shared<ComboTrialScreen>(this);
        } else if (screenSequence.back() == "ModeSelectionScreen") {
            currentScreen = std::make_shared<ModeSelectionScreen>(this);
        } else {
            currentScreen = std::make_shared<TitleScreen>(this);
        }
        
        // Main loop for current screen
        while (active && running) {
            Uint32 frameStart = SDL_GetTicks();
            
            // Update input devices
            for (auto& device : inputDeviceList) {
                device->update();
            }
            if (dummyInputDevice) {
                dummyInputDevice->update();
            }
            
            // Update camera
            camera->update(cameraFocusPoint);
            
            // Run screen loop
            currentScreen->loop();
            
            // Display
            screen->display();
            SDL_GL_SwapWindow(window);
            
            // Handle events
            eventHandler();
            
            // Frame rate limiting
            Uint32 frameTime = SDL_GetTicks() - frameStart;
            if (frameTime < 1000 / frameRate) {
                SDL_Delay((1000 / frameRate) - frameTime);
            }
        }
        
        // Clean up current screen
        currentScreen->deinit();
        screenSequence.pop_back();
        
        // Reset state
        hitstop = 0;
        cameraFocusPoint = {0.0f, 0.0f, 400.0f};
        superstop = 0;
        cameraPath.clear();
        frame = {0, 0};
        pos = {10.0f, 0.0f, 0.0f};
        drawShake = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
        showBoxes = false;
        active = true;
        recordInput = false;
        reproduceInput = false;
        activePlayers.clear();
        activeStages.clear();
    }
}

void Game::eventHandler() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                active = false;
                running = false;
                break;
                
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_0) {
                    active = false;
                    running = false;
                } else if (event.key.keysym.sym == SDLK_9) {
                    active = false;
                }
                break;
                
            case SDL_JOYDEVICEADDED:
                {
                    int deviceIndex = event.jdevice.which;
                    auto joystickDevice = std::make_shared<InputDevice>(this, 2, deviceIndex, "joystick");
                    inputDeviceList.push_back(joystickDevice);
                    
                    if (activePlayers.size() > 1) {
                        // Assign to second player if available
                        // activePlayers[1]->inputDevice = joystickDevice.get();
                    }
                }
                break;
                
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    resolution.first = event.window.data1;
                    resolution.second = event.window.data2;
                    setModeOpenGL(resolution.first, resolution.second);
                }
                break;
        }
    }
}

void Game::gameplay() {
    emuFrame++;
    
    // Update all objects
    for (auto& object : objectList) {
        object->update(cameraFocusPoint);
    }
    
    // Update hitstop
    if (hitstop > 0) {
        hitstop--;
    }
    
    // Calculate collisions
    calculateBoxesCollisions(this);
    
    // Update camera
    calculateCameraFocusPoint();
}

void Game::display() {
    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Draw all objects
    for (auto& object : objectList) {
        object->draw(screen.get(), camera->pos);
        
        if (showBoxes) {
            drawBoxes(this, object.get());
        }
    }
    
    // Draw input displays if enabled
    if (showInputs) {
        for (auto& device : inputDeviceList) {
            device->draw(screen.get(), camera->pos);
        }
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
    
    // Apply camera limits if stage is loaded
    if (!activeStages.empty()) {
        // Camera limit logic would go here
    }
    
    // Update camera focus point
    if (cameraPath.empty()) {
        cameraFocusPoint = pos;
    }
}

void Game::nextScreen(const std::vector<std::string>& newScreenSequence) {
    active = false;
    screenSequence.insert(screenSequence.end(), newScreenSequence.begin(), newScreenSequence.end());
}

void Game::cleanup() {
    // Clean up textures
    for (auto& [key, texData] : imageDict) {
        glDeleteTextures(1, &texData.id);
    }
    imageDict.clear();
    
    // Clean up sounds
    for (auto& [key, sound] : soundDict) {
        Mix_FreeChunk(sound);
    }
    soundDict.clear();
    
    // Clean up SDL
    if (glContext) {
        SDL_GL_DeleteContext(glContext);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    
    Mix_CloseAudio();
    IMG_Quit();
    SDL_Quit();
}
