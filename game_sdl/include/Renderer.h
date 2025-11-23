#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <string>
#include <vector>

class Renderer {
public:
    static void initOpenGL(int width, int height);
    static GLuint loadImagePath(const std::string& path);
    static void drawString(const std::string& text, float x, float y, float size);
    static void drawSprite(GLuint texture, float x, float y, float w, float h, bool flipX = false);
    static void checkGLError(const std::string& location);
};

#endif // RENDERER_H
