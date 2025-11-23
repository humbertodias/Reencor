#include "Game.h"
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        Game game;
        
        if (!game.initialize()) {
            std::cerr << "Failed to initialize game" << std::endl;
            return 1;
        }
        
        game.run();
        game.shutdown();
        
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}
