#include "Renderer.h"
#include <SDL2/SDL_image.h>
#include <iostream>

void Renderer::initOpenGL(int width, int height) {
    // Set up viewport
    glViewport(0, 0, width, height);
    
    // Set up projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Orthographic projection for 2D
    glOrtho(0, width, height, 0, -1000, 1000);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Enable features
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Set clear color (black)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

GLuint Renderer::loadImagePath(const std::string& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cerr << "Failed to load image: " << path << std::endl;
        return 0;
    }
    
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    GLenum format = (surface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;
    
    glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0,
                 format, GL_UNSIGNED_BYTE, surface->pixels);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    SDL_FreeSurface(surface);
    return texture;
}

void Renderer::drawSprite(GLuint texture, float x, float y, float w, float h) {
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(x + w, y);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(x + w, y + h);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(x, y + h);
    glEnd();
}

void Renderer::drawString(const std::string& text, float x, float y, float size) {
    // TODO: Implement text rendering
    // This would need a font texture atlas and character mapping
}
