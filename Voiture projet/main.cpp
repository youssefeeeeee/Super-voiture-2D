#include "Game.hpp"
#include <cstdlib> // Pour srand
#include <ctime>   // Pour time
#include <iostream> // Pour std::cout

int main() {
    // Initialiser le g�n�rateur de nombres al�atoires une seule fois au d�but du programme
    srand(static_cast<unsigned>(time(NULL)));

    Game game;

    while (game.isRunning()) {
        game.update();
        game.render();
    }

    return 0;
}