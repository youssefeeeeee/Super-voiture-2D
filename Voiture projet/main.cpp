#include "Game.hpp"
#include <cstdlib> // Pour srand
#include <ctime>   // Pour time
#include <iostream> // Pour std::cout

int main() {
    // Initialiser le générateur de nombres aléatoires une seule fois au début du programme
    srand(static_cast<unsigned>(time(NULL)));

    Game game;

    while (game.isRunning()) {
        game.update();
        game.render();
    }

    return 0;
}