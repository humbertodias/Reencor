#include "Camera.h"
#include <algorithm>

Camera::Camera(float sensitivity) 
    : sensitivity(sensitivity),
      pos({0.0f, 0.0f, 0.0f})
{
}

void Camera::update(const std::vector<float>& focusPoint) {
    if (focusPoint.size() >= 3) {
        // For now, just snap to focus point (no smooth movement)
        // Smooth movement causes issues without proper stage limits
        pos[0] = focusPoint[0];
        pos[1] = focusPoint[1];
        pos[2] = focusPoint[2];
        
        /* TODO: Re-enable smooth movement when stage limits are implemented
        // Smooth camera movement towards focus point
        pos[0] += (focusPoint[0] - pos[0]) * sensitivity;
        pos[1] += (focusPoint[1] - pos[1]) * sensitivity;
        pos[2] += (focusPoint[2] - pos[2]) * sensitivity;
        */
    }
}
