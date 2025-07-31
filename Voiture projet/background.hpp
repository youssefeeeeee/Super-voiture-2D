#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include <SFML/Graphics.hpp>
#include <string>

class Background {
public:
    Background();
    ~Background();

    void initTextures(const std::string& roadTexturePath, const std::string& farCityTexturePath);
    void initSprites(const sf::RenderWindow& window);
    void update(float deltaTime, const sf::RenderWindow& window);
    void render(sf::RenderTarget& target) const;
    float getRoadVisualHeight() const; // Hauteur visuelle de la route après scaling
    float getRoadTopY(const sf::RenderWindow& window) const; // Position Y du haut de la route

private:
    sf::Texture roadTexture;
    sf::Texture farCityTexture;

    sf::Sprite roadSprite1;
    sf::Sprite roadSprite2;
    sf::Sprite farCitySprite1;
    sf::Sprite farCitySprite2;

    float roadScaleY; // Pour stocker l'échelle Y de la route
};

#endif // BACKGROUND_HPP