#ifndef COMMON_FUNCTIONS_H
#define COMMON_FUNCTIONS_H

#include <algorithm>

class BaseActiveObject;

template<typename T>
T clamp(T value, T minVal, T maxVal) {
    return std::max(minVal, std::min(value, maxVal));
}

void updateDisplayShake(BaseActiveObject* object);

#endif // COMMON_FUNCTIONS_H
