#include "OpenGLRenderer.h"
#include <iostream>
#include <cmath>

void setModeOpenGL(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.0, static_cast<float>(width) / height, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.1f);
    
    // Lighting setup
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    GLfloat lightPos[] = {-1.0f, 1.0f, 2.0f, 0.0f};
    GLfloat lightAmbient[] = {0.9f, 0.9f, 0.9f, 1.0f};
    GLfloat lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat lightSpecular[] = {0.8f, 0.8f, 0.8f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
}

TextureData loadImagePath(const std::string& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cerr << "Failed to load image: " << path << " - " << IMG_GetError() << std::endl;
        return {0, 0, 0};
    }
    
    // Convert to RGBA format
    SDL_Surface* rgbaSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(surface);
    
    if (!rgbaSurface) {
        std::cerr << "Failed to convert surface to RGBA" << std::endl;
        return {0, 0, 0};
    }
    
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rgbaSurface->w, rgbaSurface->h,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbaSurface->pixels);
    
    TextureData texData = {textureId, rgbaSurface->w, rgbaSurface->h};
    SDL_FreeSurface(rgbaSurface);
    
    return texData;
}

void drawTexture(
    GLuint textureId,
    const std::vector<float>& pos,
    const std::vector<float>& size,
    const std::vector<bool>& flip,
    const std::vector<int>& tint,
    const std::vector<float>& angle,
    bool repeat,
    float glow,
    bool alwaysOnTop,
    bool centerOrigin
) {
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    float x = pos[0];
    float y = pos[1];
    float z = pos.size() > 2 ? pos[2] : 0.0f;
    
    if (alwaysOnTop) {
        glDisable(GL_DEPTH_TEST);
    }
    
    GLenum wrapMode = repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    
    float texScaleX = repeat ? size[0] / 100.0f : 1.0f;
    float texScaleY = repeat ? size[1] / 100.0f : 1.0f;
    
    float u1 = flip[0] ? texScaleX : 0.0f;
    float u2 = flip[0] ? 0.0f : texScaleX;
    float v1 = flip[1] ? 0.0f : texScaleY;
    float v2 = flip[1] ? texScaleY : 0.0f;
    
    glPushMatrix();
    
    if (centerOrigin) {
        glTranslatef(x, y, z);
        glRotatef(angle[0], 1, 0, 0);
        glRotatef(angle[1], 0, 1, 0);
        glRotatef(angle[2], 0, 0, 1);
        glTranslatef(-size[0] / 2.0f, -size[1] / 2.0f, 0);
    } else {
        glTranslatef(x + size[0] / 2.0f, y + size[1] / 2.0f, z);
        glRotatef(angle[0], 1, 0, 0);
        glRotatef(angle[1], 0, 1, 0);
        glRotatef(angle[2], 0, 0, 1);
        glTranslatef(-size[0] / 2.0f, -size[1] / 2.0f, 0);
    }
    
    GLfloat glowColor[] = {
        tint[0] / 255.0f * glow,
        tint[1] / 255.0f * glow,
        tint[2] / 255.0f * glow,
        1.0f
    };
    glMaterialfv(GL_FRONT, GL_EMISSION, glowColor);
    
    glColor4f(tint[0] / 255.0f, tint[1] / 255.0f, tint[2] / 255.0f, tint[3] / 255.0f);
    
    glBegin(GL_QUADS);
        glTexCoord2f(u1, v1); glVertex3f(0, size[1], 0);
        glTexCoord2f(u2, v1); glVertex3f(size[0], size[1], 0);
        glTexCoord2f(u2, v2); glVertex3f(size[0], 0, 0);
        glTexCoord2f(u1, v2); glVertex3f(0, 0, 0);
    glEnd();
    
    glPopMatrix();
    
    if (alwaysOnTop) {
        glEnable(GL_DEPTH_TEST);
    }
}

// Camera implementation
Camera::Camera(float smoothness) : smoothness(smoothness) {
    pos = {0.0f, 0.0f, 400.0f};
    shake = {0.0f, 0.0f, 0.0f};
}

void Camera::update(const std::vector<float>& targetPos) {
    // Smooth camera movement
    for (size_t i = 0; i < std::min(pos.size(), targetPos.size()); ++i) {
        pos[i] += (targetPos[i] - pos[i]) * smoothness;
    }
    
    // Apply camera position to OpenGL
    glLoadIdentity();
    glTranslatef(-pos[0] + shake[0], -pos[1] + shake[1], -pos[2] + shake[2]);
}

// Screen implementation
Screen::Screen(int width, int height) : width(width), height(height) {
    // Create framebuffer for offscreen rendering
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    // Create texture for framebuffer
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    
    // Create depth buffer
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete!" << std::endl;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Screen::~Screen() {
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &depthBuffer);
}

void Screen::display() {
    // Bind default framebuffer and render the offscreen texture
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Simple fullscreen quad rendering would go here
    // For now, we just swap buffers in the main loop
}

void Screen::clear() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
