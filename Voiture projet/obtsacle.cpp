#include "obstacle.hpp"
#include <cstdlib>  // Pour rand()
#include <iostream> // Pour std::cerr

Obstacle::Obstacle(const std::vector<sf::Texture>& availableTextures,
    const sf::RenderWindow& window,
    float roadTopY,
    float roadVisualHeight) {
    if (availableTextures.empty()) {
        std::cerr << "ERREUR: Aucune texture disponible pour créer un obstacle." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Sélectionner une texture aléatoirement
    int textureIndex = rand() % availableTextures.size();
    sprite.setTexture(availableTextures[textureIndex]);

    // Ajuster l'échelle (peut être spécifique à la texture si nécessaire)
    float scale = 0.5f;
    // Exemple: if (textureIndex == 2) scale = 0.4f; // Si la texture du camion est plus grande
    sprite.setScale(scale, scale);

    // Positionner l'obstacle sur la route
    float obstacleHeight = sprite.getGlobalBounds().height;
    float minY = roadTopY;
    float maxY = roadTopY + roadVisualHeight - obstacleHeight;

    // Assurer que minY n'est pas supérieur à maxY (peut arriver si la route est trop fine ou l'obstacle trop grand)
    if (minY > maxY) {
        //std::cerr << "Attention: minY > maxY lors du positionnement de l'obstacle. roadTopY=" << roadTopY << ", roadVisualHeight=" << roadVisualHeight << ", obstacleHeight=" << obstacleHeight << std::endl;
        maxY = minY; // Forcer maxY à être égal à minY pour éviter un crash avec rand() % 0
    }

    float spawnY = minY;
    if (maxY > minY) { // Pour éviter rand() % 0 si maxY == minY
        spawnY = static_cast<float>(minY + (rand() % static_cast<int>(maxY - minY + 1)));
    }


    sprite.setPosition(
        static_cast<float>(window.getSize().x) + 60.f, // Commence hors de l'écran à droite
        spawnY
    );
}

Obstacle::~Obstacle() {
    // Pas de ressources dynamiques directes à libérer.
}

void Obstacle::update(float deltaTime, float currentBaseSpeed, float speedMultiplier) {
    sprite.move(-deltaTime * currentBaseSpeed * speedMultiplier, 0.f);
}

void Obstacle::render(sf::RenderTarget& target) const {
    target.draw(sprite);
}

sf::Sprite& Obstacle::getSprite() {
    return sprite;
}
const sf::Sprite& Obstacle::getSprite() const { // Version const <--- AJOUTÉ
    return sprite;
}
bool Obstacle::isOffScreenLeft() const {
    return sprite.getPosition().x + sprite.getGlobalBounds().width < 0;
}