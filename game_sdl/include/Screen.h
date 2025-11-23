#ifndef SCREEN_H
#define SCREEN_H

#include <utility>

class Screen {
public:
    Screen(const std::pair<int, int>& resolution);
    ~Screen() = default;

    void display();
    void clear();

    std::pair<int, int> resolution;
};

#endif // SCREEN_H
