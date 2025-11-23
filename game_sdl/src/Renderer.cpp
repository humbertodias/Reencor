#include "Renderer.h"
#include <SDL2/SDL_image.h>
#include <iostream>

void Renderer::checkGLError(const std::string& location) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL Error at " << location << ": ";
        switch (err) {
            case GL_INVALID_ENUM:
                std::cerr << "GL_INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                std::cerr << "GL_INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                std::cerr << "GL_INVALID_OPERATION";
                break;
            case GL_OUT_OF_MEMORY:
                std::cerr << "GL_OUT_OF_MEMORY";
                break;
            default:
                std::cerr << "Unknown error code: " << err;
        }
        std::cerr << std::endl;
    }
}

void Renderer::initOpenGL(int width, int height) {
    // Set up viewport
    glViewport(0, 0, width, height);
    checkGLError("glViewport");
    
    // Set up projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Orthographic projection for 2D
    glOrtho(0, width, height, 0, -1000, 1000);
    checkGLError("glOrtho");
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Enable features
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    checkGLError("OpenGL setup");
    
    // Set clear color (black)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

GLuint Renderer::loadImagePath(const std::string& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cerr << "Failed to load image: " << path << " - " << IMG_GetError() << std::endl;
        return 0;
    }
    
    std::cout << "Loading image: " << path << " (format: " << SDL_GetPixelFormatName(surface->format->format) 
              << ", size: " << surface->w << "x" << surface->h << ")" << std::endl;
    
    // Convert surface to a consistent format (RGBA32)
    SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(surface);
    
    if (!formattedSurface) {
        std::cerr << "Failed to convert surface format for " << path << ": " << SDL_GetError() << std::endl;
        return 0;
    }
    
    GLuint texture;
    glGenTextures(1, &texture);
    checkGLError("glGenTextures");
    
    glBindTexture(GL_TEXTURE_2D, texture);
    checkGLError("glBindTexture");
    
    // Always use RGBA since we converted to RGBA32
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, formattedSurface->w, formattedSurface->h, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, formattedSurface->pixels);
    checkGLError("glTexImage2D");
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    checkGLError("glTexParameteri");
    
    SDL_FreeSurface(formattedSurface);
    
    std::cout << "Successfully created texture ID: " << texture << std::endl;
    return texture;
}

void Renderer::drawSprite(GLuint texture, float x, float y, float w, float h, bool flipX) {
    glBindTexture(GL_TEXTURE_2D, texture);
    
    float u0 = flipX ? 1.0f : 0.0f;
    float u1 = flipX ? 0.0f : 1.0f;
    
    glBegin(GL_QUADS);
        glTexCoord2f(u0, 0.0f); glVertex2f(x - w/2, y - h);
        glTexCoord2f(u1, 0.0f); glVertex2f(x + w/2, y - h);
        glTexCoord2f(u1, 1.0f); glVertex2f(x + w/2, y);
        glTexCoord2f(u0, 1.0f); glVertex2f(x - w/2, y);
    glEnd();
}

void Renderer::drawString(const std::string& text, float x, float y, float size) {
    // TODO: Implement text rendering
    // This would need a font texture atlas and character mapping
}
