#include "Background.hpp"
#include <iostream> // Pour std::cout

Background::Background() : roadScaleY(0.39f) { // Initialiser roadScaleY
    // Constructeur
}

Background::~Background() {
    // Destructeur
}

void Background::initTextures(const std::string& roadTexturePath, const std::string& farCityTexturePath) {
    if (!roadTexture.loadFromFile(roadTexturePath)) {
        std::cout << "ERREUR: Impossible de charger la texture de la route: " << roadTexturePath << std::endl;
        exit(EXIT_FAILURE);
    }
    if (!farCityTexture.loadFromFile(farCityTexturePath)) {
        std::cout << "ERREUR: Impossible de charger la texture de la ville lointaine: " << farCityTexturePath << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Background::initSprites(const sf::RenderWindow& window) {
    float windowWidth = static_cast<float>(window.getSize().x);
    float windowHeight = static_cast<float>(window.getSize().y);

    // Initialiser la route
    roadSprite1.setTexture(roadTexture);
    roadSprite2.setTexture(roadTexture);
    // L'original plaçait la route à windowHeight/2, ce qui est le centre.
    // Si la route doit commencer au centre et s'étendre vers le bas, c'est correct.
    roadSprite1.setPosition(0.f, getRoadTopY(window));
    roadSprite2.setPosition(roadSprite1.getGlobalBounds().width, getRoadTopY(window)); // Positionner côte à côte

    roadSprite1.setScale(1.f, roadScaleY);
    roadSprite2.setScale(1.f, roadScaleY);

    // Initialiser la ville lointaine
    farCitySprite1.setTexture(farCityTexture);
    farCitySprite2.setTexture(farCityTexture);
    farCitySprite1.setPosition(0.f, 0.f);
    farCitySprite2.setPosition(farCitySprite1.getGlobalBounds().width, 0.f);

    farCitySprite1.setScale(1.2f, 0.45f);
    farCitySprite2.setScale(1.2f, 0.45f);
}

void Background::update(float deltaTime, const sf::RenderWindow& window) {
    float roadWidth = roadSprite1.getGlobalBounds().width;
    float farCityWidth = farCitySprite1.getGlobalBounds().width;

    float roadScrollSpeed = 300.0f * deltaTime;
    float farCityScrollSpeed = 100.0f * deltaTime; // Vitesse plus lente pour effet de parallaxe

    // Déplacer la route
    roadSprite1.move(-roadScrollSpeed, 0.f);
    roadSprite2.move(-roadScrollSpeed, 0.f);

    // Réinitialiser la position de la route pour un défilement infini
    if (roadSprite1.getPosition().x + roadWidth <= 0.f) {
        roadSprite1.setPosition(roadSprite2.getPosition().x + roadWidth - 1.f, roadSprite1.getPosition().y); // -1.f pour éviter les gaps
    }
    if (roadSprite2.getPosition().x + roadWidth <= 0.f) {
        roadSprite2.setPosition(roadSprite1.getPosition().x + roadWidth - 1.f, roadSprite2.getPosition().y);
    }

    // Déplacer la ville lointaine
    farCitySprite1.move(-farCityScrollSpeed, 0.f);
    farCitySprite2.move(-farCityScrollSpeed, 0.f);

    // Réinitialiser la position de la ville lointaine
    if (farCitySprite1.getPosition().x + farCityWidth <= 0.f) {
        farCitySprite1.setPosition(farCitySprite2.getPosition().x + farCityWidth - 1.f, farCitySprite1.getPosition().y);
    }
    if (farCitySprite2.getPosition().x + farCityWidth <= 0.f) {
        farCitySprite2.setPosition(farCitySprite1.getPosition().x + farCityWidth - 1.f, farCitySprite2.getPosition().y);
    }
}

void Background::render(sf::RenderTarget& target) const {
    target.draw(farCitySprite1);
    target.draw(farCitySprite2);
    target.draw(roadSprite1);
    target.draw(roadSprite2);
}

float Background::getRoadVisualHeight() const {
    // Retourne la hauteur de la texture de la route après application de l'échelle.
    // Utile pour positionner des éléments sur la route.
    if (roadTexture.getSize().y > 0) {
        return static_cast<float>(roadTexture.getSize().y) * roadScaleY;
    }
    return 0.f; // Au cas où la texture ne serait pas chargée
}

float Background::getRoadTopY(const sf::RenderWindow& window) const {
    // Position Y du haut de la surface de la route
    return static_cast<float>(window.getSize().y) / 2.f;
}