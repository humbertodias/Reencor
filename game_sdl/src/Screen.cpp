#include "Screen.h"
#include <GL/gl.h>

Screen::Screen(const std::pair<int, int>& resolution) 
    : resolution(resolution)
{
}

void Screen::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Screen::display() {
    // Prepare for next frame
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
}
