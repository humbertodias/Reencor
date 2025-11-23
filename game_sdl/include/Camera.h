#ifndef CAMERA_H
#define CAMERA_H

#include <vector>

class Camera {
public:
    Camera(float sensitivity);
    ~Camera() = default;

    void update(const std::vector<float>& focusPoint);
    std::vector<float> getPos() const { return pos; }

    std::vector<float> pos;
    float sensitivity;
};

#endif // CAMERA_H
