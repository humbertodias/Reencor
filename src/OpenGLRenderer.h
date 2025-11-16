#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <vector>

struct TextureData {
    GLuint id;
    int width;
    int height;
};

// OpenGL setup
void setModeOpenGL(int width, int height);

// Texture loading
TextureData loadImagePath(const std::string& path);
TextureData fontTexture(const std::string& text, int size, int r, int g, int b);

// Drawing functions
void drawTexture(
    GLuint textureId,
    const std::vector<float>& pos,
    const std::vector<float>& size,
    const std::vector<bool>& flip = {false, false},
    const std::vector<int>& tint = {255, 255, 255, 255},
    const std::vector<float>& angle = {0, 0, 0},
    bool repeat = false,
    float glow = 0.0f,
    bool alwaysOnTop = false,
    bool centerOrigin = false
);

void drawString(
    const std::string& text,
    const std::vector<float>& pos,
    const std::vector<float>& scale,
    const std::vector<int>& color = {255, 255, 255, 255}
);

// Camera class
class Camera {
public:
    Camera(float smoothness = 0.1f);
    void update(const std::vector<float>& targetPos);
    
    std::vector<float> pos;
    float smoothness;
    std::vector<float> shake;
};

// Screen class
class Screen {
public:
    Screen(int width, int height);
    ~Screen();
    
    void display();
    void clear();
    
    int width;
    int height;
    GLuint fbo;
    GLuint texture;
    GLuint depthBuffer;
};

#endif // OPENGL_RENDERER_H
