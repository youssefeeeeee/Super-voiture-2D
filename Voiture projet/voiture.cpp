#include "Voiture.hpp"
#include <iostream> // Pour std::cerr

Voiture::Voiture() {
    // Le constructeur peut rester vide si l'initialisation principale
    // est effectuée par des méthodes dédiées comme initTexture et initSprite.
}



void Voiture::initTexture(const std::string& texturePath) {
    if (!texture.loadFromFile(texturePath)) {
        std::cerr << "ERREUR: Impossible de charger la texture de la voiture depuis " << texturePath << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Voiture::initSprite(const sf::RenderWindow& window, float roadTop, float roadHeight) {
    sprite.setTexture(texture);
    sprite.setScale(0.5f, 0.5f);

    // Positionne la voiture au centre vertical de la portion de route visible
    float voiturePosY = roadTop + (roadHeight / 2.f) - (sprite.getGlobalBounds().height / 2.f);
    sprite.setPosition(30.f, voiturePosY);
}

void Voiture::update(const sf::RenderWindow& window, float roadTop, float roadHeight) {
    // Mouvement vertical
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        sprite.move(0.f, -7.f);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        sprite.move(0.f, 7.f);
    }

    sf::Vector2f voiturePos = sprite.getPosition();
    float voitureWidth = sprite.getGlobalBounds().width;
    float voitureHeight = sprite.getGlobalBounds().height;

    // Contraintes horizontales (limites de la fenêtre)
    if (voiturePos.x < 0.f) {
        sprite.setPosition(0.f, voiturePos.y);
    }
    if (voiturePos.x + voitureWidth > window.getSize().x) {
        sprite.setPosition(window.getSize().x - voitureWidth, voiturePos.y);
    }

    // Contraintes verticales (limites de la route)
    float minY = roadTop;
    float maxY = roadTop + roadHeight - voitureHeight;

    if (voiturePos.y < minY) {
        sprite.setPosition(voiturePos.x, minY);
    }
    if (voiturePos.y > maxY) {
        sprite.setPosition(voiturePos.x, maxY);
    }
}

void Voiture::render(sf::RenderTarget& target) const {
    target.draw(sprite);
}

sf::Sprite& Voiture::getSprite() {
    return sprite;
}