#include "Game.h"
#include <iostream>
#include <exception>

int main(int argc, char* argv[]) {
    try {
        Game game;
        
        if (!game.initialize()) {
            std::cerr << "Failed to initialize game" << std::endl;
            return 1;
        }
        
        std::cout << "Starting REENCOR..." << std::endl;
        game.run();
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
